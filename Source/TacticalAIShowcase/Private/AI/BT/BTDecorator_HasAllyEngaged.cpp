#include "AI/BT/BTDecorator_HasAllyEngaged.h"
#include "AIController.h"
#include "AI/SquadSubsystem.h"

UBTDecorator_HasAllyEngaged::UBTDecorator_HasAllyEngaged()
{
	NodeName = "Has Ally Engaged";
	bNotifyTick = true;
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
}

bool UBTDecorator_HasAllyEngaged::CalculateRawConditionValue(UBehaviorTreeComponent& Owner, uint8* NodeMemory) const
{
	AAIController* AI_Controller = Owner.GetAIOwner();
	APawn* AI_Pawn = AI_Controller ? AI_Controller->GetPawn() : nullptr;

	if (!AI_Pawn)
	{
		return false;
	}

	UWorld* World = AI_Pawn->GetWorld();

	USquadSubsystem* Squad = World ? World->GetSubsystem<USquadSubsystem>() : nullptr;

	if (!Squad)
	{
		return false;
	}

	return Squad->NumberOfEngagedAllies(AI_Pawn) >= MinEngagedAllies;
}

void UBTDecorator_HasAllyEngaged::TickNode(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds)
{
	// Re-evaluate condition each tick
	// So the branch aborts when ally count crosses MinEngagedAllies
	ConditionalFlowAbort(Owner, EBTDecoratorAbortRequest::ConditionResultChanged);
}
