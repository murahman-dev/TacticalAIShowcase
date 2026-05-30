#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TacticalAIGameMode.generated.h"

/*
* Game mode class.
* Represents rules and modes related to the persistent level.
*/
UCLASS()
class TACTICALAISHOWCASE_API ATacticalAIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ATacticalAIGameMode();

protected:
	// BeginPlay
	virtual void BeginPlay() override;
};
