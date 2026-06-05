#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "BTService_SetStateTag.generated.h"

UCLASS()
class TACTICALAISHOWCASE_API UBTService_SetStateTag : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_SetStateTag();

	UPROPERTY(EditAnywhere, Category = "AI|State", meta = (Categories = "AI.State"))
	FGameplayTag StateTag;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;
};
