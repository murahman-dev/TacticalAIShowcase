#include "AI/BT/BTService_SetStateTag.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"
#include "AI/SquadSubsystem.h"

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

	AAIController* AI_Controller = Owner.GetAIOwner();
	if (!AI_Controller)
	{
		return;
	}

	// Store the tag's FName, not the FGameplayTag itself
	// Name comparison is enough for downstream queries
	if (UBlackboardComponent* BB = AI_Controller->GetBlackboardComponent())
	{
		BB->SetValueAsName(BBKeys::CurrentState, StateTag.GetTagName());
	}

	// Mirror the state into the squad so HasAllyEngaged can query it cheaply
	if (UWorld* World = AI_Controller->GetWorld())
	{
		if (USquadSubsystem* Squad = World->GetSubsystem<USquadSubsystem>())
		{
			if (APawn* AI_Pawn = AI_Controller->GetPawn())
			{
				Squad->SetState(AI_Pawn, StateTag);
			}
		}
	}
}

void UBTService_SetStateTag::OnCeaseRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(Owner, NodeMemory);

	// State left in BB and squad subsystem intentionally
	// The next active branch's service overwrites both on its OnBecomeRelevant
}
