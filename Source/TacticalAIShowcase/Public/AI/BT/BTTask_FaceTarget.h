#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FaceTarget.generated.h"

class APawn;
class UBlackboardComponent;

/*
* Behavior tree task that rotates the controlled pawn to face the
* TargetActor blackboard key.
* Succeeds when within AngleToleranceDegree of the desired yaw.
* Fails if there is no controller, pawn, or target.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTTask_FaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_FaceTarget();

	// Rotation rate (higher = snappier turn, lower = slower turn)
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "1", ClampMax = "20"))
	float RotateSpeed = 6.f;

	// Yaw delta in degrees at which the task is considered complete
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.5", ClampMax = "20"))
	float AngleToleranceDegree = 5.f;

protected:
	// Called once when the task starts
	// Returns InProgress so TickTask drives the work
	// Caches pawn and BB into NodeMemory
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;

	// Called each frame while the task is in progress
	// Interpolates rotation toward the target
	virtual void TickTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds) override;

	// Pawn, BB: cached on task start to avoid per-tick lookups
	struct FMemory
	{
		TWeakObjectPtr<APawn> Pawn;
		TWeakObjectPtr<UBlackboardComponent> BB;
	};

	// Per-instance memory
	// Each parallel BT instance gets its own FMemory block
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FMemory); }
};