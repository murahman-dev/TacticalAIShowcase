#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

/*
* AI-controlled enemy character. Auto-possessed by AEnemyAIController on spawn or
* placement.
* Holds per-instance patrol waypoints and overrides eye view point
* to return actor rotation directly for perception sight cones.
*/

UCLASS()
class TACTICALAISHOWCASE_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyCharacter();

	// Patrol waypoints in actor-local space, traversed in order with wraparound
	// MakeEditWidget shows draggable handles in the editor viewport
	// Empty array falls back to random navmesh patrol within PatrolRadius
	UPROPERTY(EditInstanceOnly, Category = "AI|Patrol", meta = (MakeEditWidget = true))
	TArray<FVector> PatrolWaypoints;

protected:
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
};