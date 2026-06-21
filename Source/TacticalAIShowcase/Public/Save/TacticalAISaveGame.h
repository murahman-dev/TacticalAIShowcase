#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TacticalAISaveGame.generated.h"

// Schema version stamped on every save.
// MigrateIfNeeded reads this on load and applies version-to-version transforms.
#define TACTICAL_AI_SAVE_VERSION 1

/*
* Per-enemy snapshot written by ATacticalAIGameMode::SaveEncounter.
* Read back on ATacticalAIGameMode::LoadEncounter.
*/
USTRUCT()
struct FEnemySnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FName Role = NAME_None;

	UPROPERTY()
	FName StateTag = NAME_None;
};

/*
* Save payload owned by ATacticalAIGameMode.
* Schema versioned via SchemaVersion + MigrateIfNeeded.
*/
UCLASS()
class TACTICALAISHOWCASE_API UTacticalAISaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 SchemaVersion = TACTICAL_AI_SAVE_VERSION;

	UPROPERTY()
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY()
	TArray<FEnemySnapshot> Enemies;

	UPROPERTY()
	FDateTime SavedAtUTC;

	void MigrateIfNeeded();
};
