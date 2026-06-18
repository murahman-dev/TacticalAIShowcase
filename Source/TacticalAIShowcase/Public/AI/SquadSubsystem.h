#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "SquadSubsystem.generated.h"

/*
* World subsystem that tracks live AI pawns alongside their
* role, state, and current target.
* Populated by AEnemyAIController OnPossess/OnUnPossess.
* Read by BTDecorator_HasAllyEngaged and BTService_SetStateTag.
*/
UCLASS()
class TACTICALAISHOWCASE_API USquadSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Adds Pawn to the registry with the given Role
	void RegisterAlly(APawn* Pawn, FGameplayTag Role);

	// Removes Pawn from the registry
	void UnregisterAlly(APawn* Pawn);

	// Updates the state tag for Pawn
	void SetState(APawn* Pawn, FGameplayTag StateTag);

	// Updates the engagement target for Pawn
	void SetTarget(APawn* Pawn, AActor* Target);

	// Returns the number of other allies in Engage or Flank state
	int32 NumberOfEngagedAllies(APawn* Asker) const;

	// Returns the target of the first other ally in
	// Engage state with a live target
	AActor* MostEngagedTarget(APawn* Asker) const;

private:
	// Per-ally record
	// Weak refs so a stale entry doesn't keep a dead pawn or target alive
	struct FAllyInfo
	{
		TWeakObjectPtr<APawn> Pawn;

		// AI.Role.* tag
		FGameplayTag Role;

		// AI.State.* tag
		// Mirrored from BB by BTService_SetStateTag
		FGameplayTag State;

		TWeakObjectPtr<AActor> Target;
	};
	TArray<FAllyInfo> Allies;
};