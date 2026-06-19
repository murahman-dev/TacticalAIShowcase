#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Suppress.generated.h"

class APawn;
class UBlackboardComponent;

/*
* Placeholder suppression task.
* Holds position for Duration seconds and periodically draws
* a debug line at the target to visualize fire.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTTask_Suppress : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_Suppress();

	// How long the AI suppresses before the task completes (seconds)
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.2", ClampMax = "5"))
	float Duration = 1.5f;

	// Yaw rotation rate while suppressing (RInterpTo speed)
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.5", ClampMax = "20.0"))
	float RotationInterpSpeed = 5.f;

protected:
	// Called once when the task starts
	// Resets per-instance counters
	// Caches pawn and BB into NodeMemory
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;

	// Called each frame while the task is in progress
	// Drives timers and fires debug shots
	virtual void TickTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory, float DeltaSeconds) override;

	// Elapsed: total time since task start (counts up toward Duration)
	// NextShot: countdown to the next debug-line draw (resets after each draw)
	// Pawn, BB: cached on task start to avoid per-tick lookups
	struct FMemory
	{
		float Elapsed = 0.f;
		float NextShot = 0.f;
		TWeakObjectPtr<APawn> Pawn;
		TWeakObjectPtr<UBlackboardComponent> BB;
	};

	// Per-instance memory
	// Each parallel BT instance gets its own FMemory block
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FMemory); }
};