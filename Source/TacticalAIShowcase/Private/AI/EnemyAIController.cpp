#include "AI/EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& Init) : Super(Init)
{
	// Enable ticking if needed for future extensions (currently not required)
	PrimaryActorTick.bCanEverTick = false;

	// Create perception component (core AI sensing system)
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	
	// Create individual sense configurations
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));

	// Assign perception component to AI controller
	SetPerceptionComponent(*PerceptionComp);

	// Apply configurable values (radius, angles, etc.)
	ApplyConfiguredSenseValues();

	// Register senses with perception system
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->ConfigureSense(*HearingConfig);
	PerceptionComp->ConfigureSense(*DamageConfig);

	// Set sight as the primary sense for decision-making
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
}

FGenericTeamId AEnemyAIController::GetGenericTeamId() const
{
	// Team 1 = enemy. Player is team 0. 255 is neutral.
	return FGenericTeamId(1);
}

void AEnemyAIController::ApplyConfiguredSenseValues()
{
	// Ensure configurations are valid before applying values
	if (!SightConfig || !HearingConfig || !DamageConfig) return;

	// Sight setup
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralHalfAngleDeg;
	SightConfig->SetMaxAge(8.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	// Hearing setup
	HearingConfig->HearingRange = HearingRadius;
	HearingConfig->SetMaxAge(6.f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Damage sense setup (used for instant reactions)
	DamageConfig->SetMaxAge(10.f);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Bind perception callback when AI takes control of a pawn
	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetSensed);
	}
}

void AEnemyAIController::OnUnPossess()
{
	// Clean up delegate binding to avoid dangling callbacks
	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIController::OnTargetSensed);
	}

	Super::OnUnPossess();
}

void AEnemyAIController::OnTargetSensed(AActor* Actor, FAIStimulus Stim)
{
	UE_LOG(LogTemp, Display, TEXT("Sensed : %s, Sense : %d, Sense Status : %d"), Actor ? *Actor->GetName() : TEXT("NONE"), Stim.Type.Index, Stim.WasSuccessfullySensed());
	// Ignore invalid actors
	if (!Actor) return;
	
	UBlackboardComponent* BB = GetBlackboardComponent();

	// Ensure blackboard exists
	if (!BB) return;
	
	// Get ID for sight sense to distinguish perception types
	const FAISenseID SightID = UAISense::GetSenseID<UAISense_Sight>();

	// Handle sight-specific logic
	if (Stim.Type == SightID)
	{
		if (Stim.WasSuccessfullySensed())
		{
			// Target is visible
			BB->SetValueAsObject(TEXT("TargetActor"), Actor);
			BB->SetValueAsVector(TEXT("LastKnownLocation"), Stim.StimulusLocation);
			BB->SetValueAsBool(TEXT("bSeesTarget"), true);
		}
		else
		{
			// Lost visual contact
			BB->SetValueAsBool(TEXT("bSeesTarget"), false);
		}
	}

	// Handle non-sight stimuli (hearing, damage, etc.)
	else if (Stim.WasSuccessfullySensed())
	{
		// Update last known position from sound/damage
		BB->SetValueAsVector(TEXT("LastKnownLocation"), Stim.StimulusLocation);
	}
}