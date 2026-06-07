#include "AI/BT/BTService_SetStateTag.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"

UBTService_SetStateTag::UBTService_SetStateTag()
{
	NodeName = TEXT("Set State Tag");

	// Notify on subtree enter/exit only 
	// State needs writing once when the branch activates, 
	// not every tick
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = false;
	bCallTickOnSearchStart = false;
}

void UBTService_SetStateTag::OnBecomeRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(Owner, NodeMemory);
	
	// Store the tag's FName, not the FGameplayTag itself
	// Name comparison is enough for downstream queries
	if (AAIController* AI_Controller = Owner.GetAIOwner())
	{
		if (UBlackboardComponent* BB = AI_Controller->GetBlackboardComponent())
		{
			BB->SetValueAsName(BBKeys::CurrentState, StateTag.GetTagName());
		}
	}	
}

void UBTService_SetStateTag::OnCeaseRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(Owner, NodeMemory);

	// CurrentState left in place intentionally
	// The next active branch's service overwrites it
}
