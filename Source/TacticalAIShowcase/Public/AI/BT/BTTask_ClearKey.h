#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearKey.generated.h"

/*
* Behavior tree task that clears a blackboard key.
* Key to clear can be picked via a dropdown in the BT editor.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTTask_ClearKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_ClearKey();

protected:
	// Key to clear, selected per-task in the BT editor
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector BlackboardKey;

	// Called when the task fires
	// Clears the selected blackboard key and returns Succeeded
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;
};
