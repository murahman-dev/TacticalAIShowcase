#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Logging/LogMacros.h"
#include "PlayerCharacter.generated.h"

// Forward declarations to reduce compile dependencies
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/*
* Base player character class.
* Represents player pawn in the game world.
*/
UCLASS(abstract)
class APlayerCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	// Spring arm boom positioning the follow camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Third-person follow camera attached to the boom socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:
	// Ground movement action (X axis strafe, Y axis forward)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	// Camera look action (X axis yaw, Y axis pitch)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	// Jump action (Started -> Jump, Completed -> StopJumping)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

public:
	// Constructor
	APlayerCharacter();

	// Setup affiliation
	virtual FGenericTeamId GetGenericTeamId() const override;
protected:
	// Bind input action delegates to character methods
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Movement handler (translates 2D input to forward/right movement)
	void Move(const FInputActionValue& Value);

	// Camera look handler (feeds yaw/pitch into controller rotation)
	void Look(const FInputActionValue& Value);

	// Jump handler (triggers ACharacter::Jump)
	void StartJumping(const FInputActionValue& Value);

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

