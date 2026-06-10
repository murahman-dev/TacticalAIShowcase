#include "AI/BT/BTTask_ClearKey.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearKey::UBTTask_ClearKey()
{
	NodeName = TEXT("Clear Blackboard Key");
}

EBTNodeResult::Type UBTTask_ClearKey::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	UBlackboardComponent* BB = Owner.GetBlackboardComponent();
	if (BB)
	{
		// SelectedKeyName is set at task init from the editor dropdown
		BB->ClearValue(BlackboardKey.SelectedKeyName);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
