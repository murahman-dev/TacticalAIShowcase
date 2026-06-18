#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasAllyEngaged.generated.h"

/*
* Gates a branch on whether at least MinEngagedAllies allies are in Engage or Flank state.
* Reads from USquadSubsystem.
* Used to gate Flanker behavior.
* Ticks so the condition re-evaluates and aborts the active branch when ally counts cross the threshold.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTDecorator_HasAllyEngaged : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasAllyEngaged();

	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "1", ClampMax = "6"))
	int32 MinEngagedAllies = 1;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& Owner, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds) override;
};
