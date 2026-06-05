#include "AI/BT/BTService_SetStateTag.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"

UBTService_SetStateTag::UBTService_SetStateTag()
{
	NodeName = TEXT("Set State Tag");
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = false;
	Interval = 1.f;
	bCallTickOnSearchStart = false;
}

void UBTService_SetStateTag::OnBecomeRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(Owner, NodeMemory);
	
	if (AAIController* AI_Controller = Owner.GetAIOwner())
		if (UBlackboardComponent* BB = AI_Controller->GetBlackboardComponent())
			BB->SetValueAsName(BBKeys::CurrentState, StateTag.GetTagName());
}

void UBTService_SetStateTag::OnCeaseRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(Owner, NodeMemory);
}
