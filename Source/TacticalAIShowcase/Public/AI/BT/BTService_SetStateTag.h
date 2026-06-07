#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "BTService_SetStateTag.generated.h"

/*
* Writes a gameplay tag into the blackboard's CurrentState key when its
* subtree becomes the active branch. 
* Lets other systems query what the AI is currently doing.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTService_SetStateTag : public UBTService
{
	GENERATED_BODY()
	
public:
	// Constructor
	UBTService_SetStateTag();

	// Tag written into the blackboard when this service activates
	UPROPERTY(EditAnywhere, Category = "AI|State", meta = (Categories = "AI.State"))
	FGameplayTag StateTag;

protected:
	// Called when this service's subtree becomes the active branch
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;

	// Called when this service's subtree is no longer active
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;
};
