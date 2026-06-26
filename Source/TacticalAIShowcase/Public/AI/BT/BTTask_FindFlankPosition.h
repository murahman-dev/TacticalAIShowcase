#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTTask_FindFlankPosition.generated.h"

class UEnvQuery;

/*
* Behavior tree task that runs an EQS query and writes the best result
* location into a blackboard Vector key.
* Latent: ExecuteTask submits the query and returns InProgress.
* OnQueryDone finishes the task.
*/
UCLASS()
class TACTICALAISHOWCASE_API UBTTask_FindFlankPosition : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_FindFlankPosition();

	// EQS query asset to run (querier resolves to the controller's pawn)
	UPROPERTY(EditAnywhere, Category = "EQS")
	TObjectPtr<UEnvQuery> Query;

	// Blackboard Vector key to receive the chosen position
	// Editor dropdown filtered to Vector keys only
	UPROPERTY(EditAnywhere, Category = "EQS")
	FBlackboardKeySelector ResultKey;

	// Side waypoint reached before ResultKey that makes the approach arc
	// Editor dropdown filtered to Vector keys only
	UPROPERTY(EditAnywhere, Category = "EQS")
	FBlackboardKeySelector WaypointKey;

	// Waypoint distance from the player
	UPROPERTY(EditAnywhere, Category = "Flank", meta = (ClampMin = "0"))
	float FlankRadius = 450.f;

	// Arc angle off the player->flanker line toward the target side
	UPROPERTY(EditAnywhere, Category = "Flank", meta = (ClampMin = "0", ClampMax = "170"))
	float ApproachAngleDegrees = 75.f;

	// New pick must be at least this far from the current flank to relocate
	UPROPERTY(EditAnywhere, Category = "Flank", meta = (ClampMin = "0"))
	float ReflankThreshold = 300.f;

	// Player camera aim must be within this half-angle of the flanker to relocate
	UPROPERTY(EditAnywhere, Category = "Flank", meta = (ClampMin = "0", ClampMax = "90"))
	float FacingConeHalfAngleDegrees = 45.f;

protected:
	// Called when the task fires
	// Submits the EQS query and returns InProgress
	// OnQueryDone completes the task
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;

	// Reset CachedOwner
	// OnQueryDone's IsValid guard absorbs the late callback
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) override;

private:
	// EQS callback fired when the query finishes
	void OnQueryDone(TSharedPtr<FEnvQueryResult> Result);

	// Player camera aim points within the facing cone of the flanker
	bool IsPlayerFacing(AActor* Player, APawn* Flanker) const;

	// Holds the BT component across the async query window
	// TWeakObjectPtr so the task survives the BT being destroyed mid-query
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwner;
};