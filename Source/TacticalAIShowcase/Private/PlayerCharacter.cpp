#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Perception/AISense_Hearing.h"
#include "TacticalAIGameMode.h"

APlayerCharacter::APlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Walking speed based on stance
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Movement noise emitter, fires every NoiseEmitInterval while moving
	GetWorldTimerManager().SetTimer(
		NoiseTimerHandle,
		this,
		&APlayerCharacter::EmitMovementNoise,
		NoiseEmitInterval,
		true);
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	// Team 0 = player. Enemy is team 1. 255 is neutral.
	return FGenericTeamId(0);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleCrouch);
		EnhancedInputComponent->BindAction(SaveAction, ETriggerEvent::Started, this, &APlayerCharacter::SaveEncounter);
		EnhancedInputComponent->BindAction(LoadAction, ETriggerEvent::Started, this, &APlayerCharacter::LoadEncounter);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (GetController() != nullptr)
	{
		// Yaw-only rotation so movement stays on the ground plane
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Forward/right derived from yaw rotation
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::ToggleCrouch()
{
	// bIsCrouched is set by Crouch()/UnCrouch() on ACharacter
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacter::EmitMovementNoise()
{
	if (GetVelocity().IsNearlyZero())
	{
		return;
	}

	// Crouching reduces loudness
	// But close-range detection still works
	const float Loudness = bIsCrouched ? CrouchNoiseLoudness : WalkNoiseLoudness;
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this);
}

void APlayerCharacter::SaveEncounter()
{
	ATacticalAIGameMode* GM = Cast<ATacticalAIGameMode>(UGameplayStatics::GetGameMode(this));

	if (!GM)
	{
		return;
	}

	GM->SaveEncounter(TacticalAISaveSlots::Default);
}

void APlayerCharacter::LoadEncounter()
{
	ATacticalAIGameMode* GM = Cast<ATacticalAIGameMode>(UGameplayStatics::GetGameMode(this));

	if (!GM)
	{
		return;
	}

	GM->LoadEncounter(TacticalAISaveSlots::Default);
}