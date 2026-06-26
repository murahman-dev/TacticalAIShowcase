#include "AI/BT/BTDecorator_HasAllyEngaged.h"
#include "AIController.h"
#include "AI/SquadSubsystem.h"

UBTDecorator_HasAllyEngaged::UBTDecorator_HasAllyEngaged()
{
	NodeName = "Has Ally Engaged";
	bNotifyTick = true;

	// Per-AI instance so the held value is not shared across enemies
	bCreateNodeInstance = true;

	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
}

bool UBTDecorator_HasAllyEngaged::ComputeRaw(UBehaviorTreeComponent& Owner) const
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

bool UBTDecorator_HasAllyEngaged::CalculateRawConditionValue(UBehaviorTreeComponent& Owner, uint8* NodeMemory) const
{
	// First check sets the held value from the live count
	if (!bGateInitialized)
	{
		bGate = ComputeRaw(Owner);
		bGateInitialized = true;
		PendingTime = 0.f;
	}

	// Return the held value
	// The tick keeps it updated
	return bGate;
}

void UBTDecorator_HasAllyEngaged::TickNode(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds)
{
	const bool bRaw = ComputeRaw(Owner);

	if (bRaw == bGate)
	{
		// Live count agrees with the held value
		// Reset the timer
		PendingTime = 0.f;
		return;
	}

	// Live count differs
	// Wait until it holds for CommitDelay before flipping
	PendingTime += DeltaSeconds;
	if (PendingTime >= CommitDelay)
	{
		bGate = bRaw;
		PendingTime = 0.f;

		// Held value changed
		// Abort the active branch so the selector re-picks
		ConditionalFlowAbort(Owner, EBTDecoratorAbortRequest::ConditionResultChanged);
	}
}