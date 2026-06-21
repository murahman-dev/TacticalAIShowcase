#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TacticalAIGameMode.generated.h"

/*
* Save slot identifiers shared with PlayerCharacter quicksave/quickload.
* extern/cpp split (definition in .cpp).
*/
namespace TacticalAISaveSlots
{
	extern const FString Default;
}

/*
* Game mode class.
* Represents rules and modes related to the persistent level.
* Owns SaveEncounter/LoadEncounter, invoked from PlayerCharacter input.
*/
UCLASS()
class TACTICALAISHOWCASE_API ATacticalAIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ATacticalAIGameMode();

	// Snapshots player position and all live AI BB state into a UTacticalAISaveGame
	// and writes it to disk under the given slot name
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveEncounter(const FString& Slot);

	// Reads the slot from disk, migrates schema if needed, restores player location
	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadEncounter(const FString& Slot);
};
