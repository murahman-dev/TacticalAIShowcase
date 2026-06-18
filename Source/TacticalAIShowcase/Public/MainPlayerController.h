#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

// Forward declarations to reduce compile dependencies
class UInputMappingContext;
class UUserWidget;

/*
* Base player controller.
* Adds DefaultMappingContexts to the Enhanced Input subsystem on SetupInputComponent.
* Subclassed in BP for game-specific input.
*/
UCLASS(abstract)
class AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Input mapping contexts applied on setup
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	// Applies all DefaultMappingContexts to the Enhanced Input subsystem for local players
	virtual void SetupInputComponent() override;
};
