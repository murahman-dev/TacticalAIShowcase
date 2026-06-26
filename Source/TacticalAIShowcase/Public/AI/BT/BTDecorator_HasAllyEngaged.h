#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasAllyEngaged.generated.h"

/*
* Gates a branch on whether at least MinEngagedAllies allies are in Engage or Flank state.
* Reads from USquadSubsystem.
* Used to gate Flanker behavior.
* Ticks so the condition re-evaluates, and aborts the active branch when the count holds across the threshold for CommitDelay.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTDecorator_HasAllyEngaged : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasAllyEngaged();

	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "1", ClampMax = "6"))
	int32 MinEngagedAllies = 1;

	// Count must hold across the threshold this long before the gate flips
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0"))
	float CommitDelay = 0.5f;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& Owner, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// Live count test, count >= MinEngagedAllies
	bool ComputeRaw(UBehaviorTreeComponent& Owner) const;

	// Held value the tree sees
	// Flips only after CommitDelay
	// Mutable so the const check can set it on the first evaluation
	mutable bool bGate = false;
	mutable bool bGateInitialized = false;

	// Time the live count has differed from the held value
	mutable float PendingTime = 0.f;
};