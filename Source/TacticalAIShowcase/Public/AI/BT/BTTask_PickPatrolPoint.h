#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PickPatrolPoint.generated.h"

/*
* Behavior tree task that picks a random navmesh-reachable point within
* PatrolRadius of the AI's home location and writes it to the
* PatrolDestination blackboard key.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTTask_PickPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_PickPatrolPoint();

	// Maximum distance from home the AI can wander (cm)
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "500", ClampMax = "2000"))
	float PatrolRadius = 1500.f;

protected:
	// Called when the task fires
	// Picks a reachable point and writes it to BBKeys::PatrolDestination
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;
};
