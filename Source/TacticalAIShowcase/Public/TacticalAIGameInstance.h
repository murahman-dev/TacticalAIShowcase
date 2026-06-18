#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TacticalAIGameInstance.generated.h"

/*
* Persists across level transitions.
* Hosts the team attitude solver (Friendly / Neutral / Hostile).
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
};