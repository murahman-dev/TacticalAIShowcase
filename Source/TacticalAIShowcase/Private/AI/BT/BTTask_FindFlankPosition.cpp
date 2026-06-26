#include "AI/BT/BTTask_FindFlankPosition.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "NavigationSystem.h"
#include "AI/BlackboardKeys.h"

UBTTask_FindFlankPosition::UBTTask_FindFlankPosition()
{
	NodeName = TEXT("Find Flank Position");

	// Each AI gets its own task instance so CachedOwner is per-AI
	// Without this, the task is a shared CDO and CachedOwner
	// is shared across enemies
	bCreateNodeInstance = true;

	// Editor dropdown for ResultKey shows only Vector keys
	ResultKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFlankPosition, ResultKey));
	WaypointKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFlankPosition, WaypointKey));
}

EBTNodeResult::Type UBTTask_FindFlankPosition::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	AAIController* AI_Controller = Owner.GetAIOwner();
	if (!AI_Controller || !Query)
	{
		return EBTNodeResult::Failed;
	}

	// Querier = pawn for spatial context and BB access via custom contexts
	FEnvQueryRequest Req(Query, AI_Controller->GetPawn());

	// Cache owner for the async callback
	CachedOwner = &Owner;

	// SingleResult: best-scored item from the query
	Req.Execute(EEnvQueryRunMode::SingleResult, this, &UBTTask_FindFlankPosition::OnQueryDone);

	// Latent: OnQueryDone finishes the task
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_FindFlankPosition::AbortTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	// IsValid guard in OnQueryDone absorbs the late callback
	CachedOwner.Reset();
	return EBTNodeResult::Aborted;
}

void UBTTask_FindFlankPosition::OnQueryDone(TSharedPtr<FEnvQueryResult> Result)
{
	UBehaviorTreeComponent* Owner = CachedOwner.Get();

	if (!Owner)
	{
		// BT destroyed between Execute and callback
		return;
	}

	if (!Result.IsValid() || !Result->IsSuccessful() || Result->Items.Num() == 0)
	{
		FinishLatentTask(*Owner, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = Owner->GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(*Owner, EBTNodeResult::Failed);
		return;
	}

	const FVector FlankPoint = Result->GetItemAsLocation(0);

	AAIController* AI_Controller = Owner->GetAIOwner();
	APawn* Pawn = AI_Controller ? AI_Controller->GetPawn() : nullptr;
	AActor* Player = Cast<AActor>(BB->GetValueAsObject(BBKeys::TargetActor));

	// Relocate only when the new pick is far and the player is facing the flanker
	// a sidestep drifts the pick but does not turn toward him, so he holds
	const FVector CurrentFlank = BB->GetValueAsVector(ResultKey.SelectedKeyName);
	if (!CurrentFlank.IsNearlyZero())
	{
		const bool bFarEnough = FVector::DistSquared2D(FlankPoint, CurrentFlank) >= FMath::Square(ReflankThreshold);
		const bool bFacing = Pawn && Player && IsPlayerFacing(Player, Pawn);
		if (!bFarEnough || !bFacing)
		{
			// Pick barely moved or player not facing
			// So collapse the waypoint
			BB->SetValueAsVector(WaypointKey.SelectedKeyName, CurrentFlank);
			FinishLatentTask(*Owner, EBTNodeResult::Succeeded);
			return;
		}
	}

	BB->SetValueAsVector(ResultKey.SelectedKeyName, FlankPoint);

	// Waypoint defaults to the flank point
	// Missing player or pawn skips the arc
	FVector Waypoint = FlankPoint;

	if (Pawn && Player)
	{
		const FVector PlayerLoc = Player->GetActorLocation();
		const FVector ToFlanker = (Pawn->GetActorLocation() - PlayerLoc).GetSafeNormal2D();
		const FVector ToTarget = (FlankPoint - PlayerLoc).GetSafeNormal2D();

		// Zero on a degenerate vector
		// Skip the arc if either is flat
		if (!ToFlanker.IsNearlyZero() && !ToTarget.IsNearlyZero())
		{
			// Cross product Z gives the side the flank point is on
			const float Side = FMath::Sign(FVector::CrossProduct(ToFlanker, ToTarget).Z);

			// Rotate player->flanker to the target side 
			// Waypoint on the flank not the front
			const FVector ArcDir = ToFlanker.RotateAngleAxis(ApproachAngleDegrees * Side, FVector::UpVector);
			Waypoint = PlayerLoc + ArcDir * FlankRadius;

			if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(Owner->GetWorld()))
			{
				FNavLocation Projected;
				if (Nav->ProjectPointToNavigation(Waypoint, Projected, FVector(250.f, 250.f, 400.f)))
				{
					Waypoint = Projected.Location;
				}
			}
		}
	}

	BB->SetValueAsVector(WaypointKey.SelectedKeyName, Waypoint);

	FinishLatentTask(*Owner, EBTNodeResult::Succeeded);
}

bool UBTTask_FindFlankPosition::IsPlayerFacing(AActor* Player, APawn* Flanker) const
{
	const APawn* PlayerPawn = Cast<APawn>(Player);
	const AController* PC = PlayerPawn ? PlayerPawn->GetController() : nullptr;

	// Aim from control rotation (camera)
	const FVector AimFlat = (PC ? PC->GetControlRotation().Vector() : Player->GetActorForwardVector()).GetSafeNormal2D();
	const FVector ToFlankerFlat = (Flanker->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal2D();

	// Zero on a degenerate vector
	// Return false if either is flat
	if (AimFlat.IsNearlyZero() || ToFlankerFlat.IsNearlyZero())
	{
		return false;
	}

	// Dot vs cos(half-angle) facing cone
	const float CosLimit = FMath::Cos(FMath::DegreesToRadians(FacingConeHalfAngleDegrees));
	return FVector::DotProduct(AimFlat, ToFlankerFlat) >= CosLimit;
}