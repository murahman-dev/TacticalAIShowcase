#include "AI/BT/BTTask_Suppress.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"
#include "Perception/AISense_Hearing.h"

namespace
{
	// Visual fire rate for the placeholder (seconds between line draws)
	constexpr float ShotInterval = 0.25f;

	// Approximate chest height of a default ACharacter capsule (cm above feet)
	constexpr float DebugLineChestHeight = 60.f;
}

UBTTask_Suppress::UBTTask_Suppress()
{
	NodeName = TEXT("Suppress (Placeholder)");

	// Latent task: ExecuteTask returns InProgress
	// TickTask drives the timer
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Suppress::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	// Per-instance memory, reset on each task start
	FMemory* M = reinterpret_cast<FMemory*>(NodeMemory);
	M->Elapsed = 0.f;
	M->NextShot = 0.f;

	// Cache pawn and BB to avoid per-tick lookups
	AAIController* AI_Controller = Owner.GetAIOwner();
	M->Pawn = AI_Controller ? AI_Controller->GetPawn() : nullptr;
	M->BB = Owner.GetBlackboardComponent();
	if (!M->Pawn.IsValid() || !M->BB.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_Suppress::TickTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds)
{
	FMemory* M = reinterpret_cast<FMemory*>(NodeMemory);
	M->Elapsed += DeltaSeconds;
	M->NextShot -= DeltaSeconds;

	APawn* AI_Pawn = M->Pawn.Get();
	UBlackboardComponent* BB = M->BB.Get();
	if (!AI_Pawn || !BB)
	{
		// Pawn or BB destroyed mid-task (death, level transition)
		FinishLatentTask(Owner, EBTNodeResult::Failed);
		return;
	}

	if (AActor* Target = Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor)))
	{
		// Continuous facing keeps the perception cone tracking the target during suppression
		// So the player can not quickly walk around the AI and slip out of cone permanently
		const FVector ToTargetDir = (Target->GetActorLocation() - AI_Pawn->GetActorLocation()).GetSafeNormal2D();
		if (!ToTargetDir.IsNearlyZero())
		{
			const FRotator TargetYaw(0.f, ToTargetDir.Rotation().Yaw, 0.f);
			const FRotator NewRot = FMath::RInterpTo(AI_Pawn->GetActorRotation(), TargetYaw, DeltaSeconds, RotationInterpSpeed);
			AI_Pawn->SetActorRotation(NewRot);
		}

		if (M->NextShot <= 0.f)
		{
			UWorld* World = AI_Pawn->GetWorld();

			// Gunfire is a hearing stimulus for other AI
			// Target as instigator so bDetectEnemies filter accepts it
			UAISense_Hearing::ReportNoiseEvent(
				World,
				Target->GetActorLocation(),
				1.5f,
				Target,
				0.f);

#if !UE_BUILD_SHIPPING
			// Chest-height debug line for placeholder fire visualization
			DrawDebugLine(
				World,
				AI_Pawn->GetActorLocation() + FVector(0, 0, DebugLineChestHeight),
				Target->GetActorLocation() + FVector(0, 0, DebugLineChestHeight),
				FColor::Red,
				false,
				0.2f,
				0,
				1.5f);
#endif

			// 4 visualized shots per second at ShotInterval = 0.25
			M->NextShot = ShotInterval;
		}
	}

	// Duration check sits outside the target-valid guard
	// Task still completes if the target disappears mid-suppress
	if (M->Elapsed >= Duration)
	{
		FinishLatentTask(Owner, EBTNodeResult::Succeeded);
	}
}