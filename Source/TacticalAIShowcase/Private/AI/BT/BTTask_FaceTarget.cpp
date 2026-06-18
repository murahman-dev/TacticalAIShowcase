#include "AI/BT/BTTask_FaceTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_FaceTarget::UBTTask_FaceTarget()
{
	NodeName = TEXT("Face Target");

	// Latent task: ExecuteTask returns InProgress
	// TickTask drives convergence to the target yaw
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FaceTarget::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	FMemory* M = reinterpret_cast<FMemory*>(NodeMemory);

	// Cache pawn and BB to avoid per-tick lookups
	AAIController* AI_Controller = Owner.GetAIOwner();
	M->Pawn = AI_Controller ? AI_Controller->GetPawn() : nullptr;
	M->BB = Owner.GetBlackboardComponent();
	if (!M->Pawn.IsValid() || !M->BB.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	// Defer all work to TickTask
	// Returning InProgress keeps the task active across frames
	return EBTNodeResult::InProgress;
}

void UBTTask_FaceTarget::TickTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds)
{
	FMemory* M = reinterpret_cast<FMemory*>(NodeMemory);
	APawn* AI_Pawn = M->Pawn.Get();
	UBlackboardComponent* BB = M->BB.Get();
	if (!AI_Pawn || !BB)
	{
		// Pawn or BB destroyed mid-task (death, level transition)
		FinishLatentTask(Owner, EBTNodeResult::Failed);
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor));
	if (!Target)
	{
		FinishLatentTask(Owner, EBTNodeResult::Failed);
		return;
	}

	// Yaw-only rotation
	// Keep current pitch and roll so the pawn stays upright
	// instead of tilting to look at a target that is taller or shorter
	const FRotator Look = UKismetMathLibrary::FindLookAtRotation(AI_Pawn->GetActorLocation(), Target->GetActorLocation());
	FRotator Current = AI_Pawn->GetActorRotation();
	FRotator Goal = Current;
	Goal.Yaw = Look.Yaw;

	// RInterpTo handles the 180-degree yaw seam correctly via
	// internal angle normalization
	const FRotator NewRot = FMath::RInterpTo(Current, Goal, DeltaSeconds, RotateSpeed);
	AI_Pawn->SetActorRotation(NewRot);

	// FindDeltaAngleDegrees returns a signed [-180, 180] difference
	// Abs gives the angular distance
	const float DeltaYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(NewRot.Yaw, Goal.Yaw));
	if (DeltaYaw <= AngleToleranceDegree)
	{
		FinishLatentTask(Owner, EBTNodeResult::Succeeded);
	}
}