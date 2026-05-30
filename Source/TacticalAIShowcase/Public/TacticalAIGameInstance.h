#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TacticalAIGameInstance.generated.h"

/*
* Game instance class.
* Used in places that need to outlive the level transition
* or similar scenarios.
*/
UCLASS()
class TACTICALAISHOWCASE_API UTacticalAIGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Constructor
	UTacticalAIGameInstance();

protected:
	// Registers the global team attitude solver (Friendly/Neutral/Hostile logic)
	virtual void Init() override;

	// Counterpart to Init, cleans up states on process exit
	virtual void Shutdown() override;
};