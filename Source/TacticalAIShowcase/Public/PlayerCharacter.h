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

/*
* Player-controlled character with third-person camera and Enhanced Input handlers.
* Implements IGenericTeamAgentInterface as team 0 for AI perception affiliation.
* Movement emits hearing stimuli on a timer.
* Crouch toggle reduces loudness so detection range shrinks.
*/
UCLASS(abstract)
class TACTICALAISHOWCASE_API APlayerCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	// Spring arm boom positioning the follow camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Third-person follow camera attached to the boom socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

protected:
	// Ground movement action (X axis strafe, Y axis forward)
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// Camera look action (X axis yaw, Y axis pitch)
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// Jump action (Started -> Jump, Completed -> StopJumping)
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	// Crouch toggle (Started -> ToggleCrouch)
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	// Quicksave action (Started -> SaveEncounter)
	UPROPERTY(EditAnywhere, Category = "Save")
	TObjectPtr<UInputAction> SaveAction;

	// Quickload action (Started -> LoadEncounter)
	UPROPERTY(EditAnywhere, Category = "Save")
	TObjectPtr<UInputAction> LoadAction;

	// Hearing stimulus loudness while moving
	// Different for walking and crouching
	UPROPERTY(EditAnywhere, Category = "Combat|Stealth", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float WalkNoiseLoudness = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Stealth", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float CrouchNoiseLoudness = 0.3f;

	// Seconds between movement noise events
	UPROPERTY(EditAnywhere, Category = "Combat|Stealth", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float NoiseEmitInterval = 0.4f;

public:
	// Constructor
	APlayerCharacter();

	// IGenericTeamAgentInterface team ID getter used by
	// perception affiliation filtering
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	// Starts the movement noise timer
	virtual void BeginPlay() override;

	// Bind input action delegates to character methods
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Translates 2D input vector into world-space forward/right movement
	// Uses controller yaw only to keep movement on the ground plane
	void Move(const FInputActionValue& Value);

	// Feeds input axes into controller yaw (X) and pitch (Y) rotation
	void Look(const FInputActionValue& Value);

	// Toggles between crouched and standing via ACharacter::Crouch/UnCrouch
	void ToggleCrouch();

	// Reports hearing stimulus when moving
	void EmitMovementNoise();

	// Input handler: routes quicksave to the GameMode using the default slot
	void SaveEncounter();

	// Input handler: routes quickload to the GameMode using the default slot
	void LoadEncounter();

private:
	FTimerHandle NoiseTimerHandle;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}
	FORCEINLINE class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}
};