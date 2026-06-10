#include "AI/BT/BTTask_PickPatrolPoint.h"
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

	// V1 is the concrete navsystem
	// UNavigationSystemBase is abstract
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(Owner.GetWorld());
	if (!Nav)
	{
		return EBTNodeResult::Failed;
	}

	const FVector Home = BB->GetValueAsVector(BBKeys::HomeLocation);

	FNavLocation Out;
	if (Nav->GetRandomReachablePointInRadius(Home, PatrolRadius, Out))
	{
		BB->SetValueAsVector(BBKeys::PatrolDestination, Out.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
