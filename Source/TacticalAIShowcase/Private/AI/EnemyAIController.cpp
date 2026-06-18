#include "AI/EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/SquadSubsystem.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& Init) : Super(Init)
{
	// AI is event-driven (perception delegates + BT)
	// AEnemyCharacter overrides GetActorEyesViewPoint so sight perception
	// uses actor rotation directly and we don't need Tick
	PrimaryActorTick.bCanEverTick = false;

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));

	SetPerceptionComponent(*PerceptionComp);

	ApplyConfiguredSenseValues();

	// Register senses with perception system
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->ConfigureSense(*HearingConfig);
	PerceptionComp->ConfigureSense(*DamageConfig);

	// Sight sense is dominant
	// Perception promotes its data over hearing/damage
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
}

void AEnemyAIController::ApplyDebugSettings(const UAIDebugSettings& DebugSettings)
{
	if (!SightConfig || !HearingConfig || !DamageConfig)
	{
		return;
	}

	SightConfig->SightRadius = DebugSettings.PendingSightRange;
	SightConfig->LoseSightRadius = DebugSettings.PendingLoseSightRange;
	SightConfig->PeripheralVisionAngleDegrees = DebugSettings.PendingHalfFOVDegree;
	HearingConfig->HearingRange = DebugSettings.PendingHearingRange;

	// Tell perception to reapply sense configs after the field updates above
	PerceptionComp->RequestStimuliListenerUpdate();
}

FGenericTeamId AEnemyAIController::GetGenericTeamId() const
{
	// Team 1 = enemy. Player is team 0. 255 is neutral.
	return FGenericTeamId(1);
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (OtherPawn->IsPlayerControlled())
		{
			return ETeamAttitude::Hostile;
		}
		return ETeamAttitude::Friendly;
	}
	return ETeamAttitude::Neutral;
}

void AEnemyAIController::ApplyConfiguredSenseValues()
{
	// Skip if any config failed to allocate during CDO setup
	if (!SightConfig || !HearingConfig || !DamageConfig)
	{
		return;
	}

	// Sight setup
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralHalfAngleDeg;
	SightConfig->SetMaxAge(8.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1000.f;

	// Detect only hostiles
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	// Hearing setup
	HearingConfig->HearingRange = HearingRadius;
	HearingConfig->SetMaxAge(6.f);

	// Detect only hostiles
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

	// Damage stimuli persist 10s on the perception system
	DamageConfig->SetMaxAge(10.f);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Bind perception after Super so PerceptionComp is alive
	// Binding earlier can miss the first stimulus
	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetSensed);
	}

	// Register with squad before BT starts
	// So a service's first SetState lands on an existing entry
	if (UWorld* World = GetWorld())
	{
		if (USquadSubsystem* Squad = World->GetSubsystem<USquadSubsystem>())
		{
			Squad->RegisterAlly(InPawn, DefaultRole);
		}
	}

	// Skip if no BT is assigned
	// Controller still runs perception above
	if (BehaviorTreeAsset)
	{
		// GetBlackboardComponent returns null on first possession
		// UseBlackboard below creates the BB and writes the handle back through the out-param
		UBlackboardComponent* BB = GetBlackboardComponent();

		if (!BB && BehaviorTreeAsset->BlackboardAsset)
		{
			UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BB);
		}

		// Seed BB before RunBehaviorTree so the first BT tick reads written values
		if (BB)
		{
			BB->SetValueAsVector(BBKeys::HomeLocation, InPawn->GetActorLocation());
			BB->SetValueAsBool(BBKeys::bSeesTarget, false);
			BB->SetValueAsName(BBKeys::Role, DefaultRole.IsValid() ? DefaultRole.GetTagName() : NAME_None);
		}

		// Start BT after all dependencies are ready
		// (BB seeds, squad entry, perception binding)
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AEnemyAIController::OnUnPossess()
{
	// Clean up delegate binding to avoid dangling callbacks
	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIController::OnTargetSensed);
	}

	if (UWorld* World = GetWorld())
	{
		if (USquadSubsystem* Squad = World->GetSubsystem<USquadSubsystem>())
		{
			if (APawn* AI_Pawn = GetPawn())
			{
				Squad->UnregisterAlly(AI_Pawn);
			}
		}
	}

	Super::OnUnPossess();
}

void AEnemyAIController::OnTargetSensed(AActor* Actor, FAIStimulus Stim)
{
	// Ignore invalid actors
	if (!Actor)
	{
		return;
	}

	UBlackboardComponent* BB = GetBlackboardComponent();

	// BB can be null during the unpossess gap between
	// BB destruction and delegate unbind
	if (!BB)
	{
		return;
	}

	// Sense ID compare is the standard branch pattern
	// (Stim.Type is FAISenseID, not a class)
	const FAISenseID SightID = UAISense::GetSenseID<UAISense_Sight>();

	if (Stim.Type == SightID)
	{
		if (Stim.WasSuccessfullySensed())
		{
			BB->SetValueAsObject(BBKeys::TargetActor, Actor);
			BB->SetValueAsVector(BBKeys::LastKnownLocation, Stim.StimulusLocation);
			BB->SetValueAsBool(BBKeys::bSeesTarget, true);
		}
		else
		{
			BB->SetValueAsBool(BBKeys::bSeesTarget, false);
		}
	}

	// Hearing/damage: update LastKnownLocation on positive stimulus only
	// Losses don't clear it (preserves last heard/hit position for investigation)
	else if (Stim.WasSuccessfullySensed())
	{
		BB->SetValueAsVector(BBKeys::LastKnownLocation, Stim.StimulusLocation);
	}
}