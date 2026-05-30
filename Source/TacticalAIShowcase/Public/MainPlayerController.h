#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

// Forward declarations to reduce compile dependencies
class UInputMappingContext;
class UUserWidget;

/*
* Base player controller class.
* Binded to player character class in the game world.
*/
UCLASS(abstract)
class AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//Constructor
	AMainPlayerController();

protected:
	// Input mapping contexts applied on setup
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	// BeginPlay
	virtual void BeginPlay() override;

	// Applies all DefaultMappingContexts to the Enhanced Input subsystem for local players
	virtual void SetupInputComponent() override;
};
