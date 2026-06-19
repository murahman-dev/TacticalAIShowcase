#include "AI/BT/BTTask_PickPatrolPoint.h"
#include "AIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"
#include "NavigationSystem.h"

UBTTask_PickPatrolPoint::UBTTask_PickPatrolPoint()
{
	NodeName = TEXT("Pick Patrol Point");
}

EBTNodeResult::Type UBTTask_PickPatrolPoint::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	UBlackboardComponent* BB = Owner.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AI_Controller = Owner.GetAIOwner();
	AEnemyCharacter* Enemy = AI_Controller ? Cast<AEnemyCharacter>(AI_Controller->GetPawn()) : nullptr;
	const FVector Home = BB->GetValueAsVector(BBKeys::HomeLocation);

	// Designed waypoints take precedence over random patrol
	if (Enemy && Enemy->PatrolWaypoints.Num() > 0)
	{
		// Wrap on read so a stale BB value past the array end doesn't crash
		const int32 Raw = BB->GetValueAsInt(BBKeys::CurrentPatrolIndex);
		const int32 Index = Raw % Enemy->PatrolWaypoints.Num();
		const FVector Target = Home + Enemy->PatrolWaypoints[Index];

		// Raw waypoint, no navmesh snapping
		// Designer is expected to place waypoints on the navmesh
		BB->SetValueAsVector(BBKeys::PatrolDestination, Target);
		BB->SetValueAsInt(BBKeys::CurrentPatrolIndex, (Index + 1) % Enemy->PatrolWaypoints.Num());
		return EBTNodeResult::Succeeded;
	}

	// Random fallback for enemies without designed routes
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(Owner.GetWorld());
	if (!Nav)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation Out;
	if (Nav->GetRandomReachablePointInRadius(Home, PatrolRadius, Out))
	{
		BB->SetValueAsVector(BBKeys::PatrolDestination, Out.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}