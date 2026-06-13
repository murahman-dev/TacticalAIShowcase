#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTTask_FindFlankPosition.generated.h"

class UEnvQuery;

/*
* Behavior tree task that runs an EQS query and writes the best result
* location into a blackboard Vector key.
* Latent: ExecuteTask submits the query and returns InProgress.
* OnQueryDone finishes the task.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTTask_FindFlankPosition : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_FindFlankPosition();

	// EQS query asset to run (querier resolves to the controller's pawn)
	UPROPERTY(EditAnywhere, Category = "EQS")
	TObjectPtr<UEnvQuery> Query;

	// Blackboard Vector key to receive the chosen position
	// Editor dropdown filtered to Vector keys only
	UPROPERTY(EditAnywhere, Category = "EQS")
	FBlackboardKeySelector ResultKey;

protected:
	// Called when the task fires
	// Submits the EQS query and returns InProgress
	// OnQueryDone completes the task
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;

private:
	// EQS callback fired when the query finishes
	void OnQueryDone(TSharedPtr<FEnvQueryResult> Result);

	// Holds the BT component across the async query window
	// TWeakObjectPtr so the task survives the BT being destroyed mid-query
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwner;
};
