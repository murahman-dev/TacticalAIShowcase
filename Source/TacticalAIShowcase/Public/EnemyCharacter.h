#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

/*
* Base enemy character class.
* Represents AI-controlled enemies in the game world.
*/

UCLASS()
class TACTICALAISHOWCASE_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Default constructor
	AEnemyCharacter();

protected:
	// Called when the game starts or the actor is spawned
	virtual void BeginPlay() override;
};