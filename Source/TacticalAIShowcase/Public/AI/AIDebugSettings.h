#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AIDebugSettings.generated.h"

/*
* Tuner state shared between SAIDebugPanel and AEnemyAIController.
* Pending* fields written on slider drag, read live by DrawHUD (cone preview).
* ApplyToAllAI applies them on live AEnemyAIControllers on button click.
*/
UCLASS()
class TACTICALAISHOWCASE_API UAIDebugSettings : public UObject
{
	GENERATED_BODY()

public:
	// Sight radius (cm)
	// Apply -> SightConfig::SightRadius
	UPROPERTY()
	float PendingSightRange = 1500.f;

	// Lose-sight radius (cm)
	// Apply -> SightConfig::LoseSightRadius
	UPROPERTY()
	float PendingLoseSightRange = 2000.f;

	// Half-angle (degrees, not full FOV)
	// Apply -> SightConfig::PeripheralVisionAngleDegrees
	UPROPERTY()
	float PendingHalfFOVDegree = 45.f;

	// Hearing radius (cm)
	// Apply -> HearingConfig::HearingRange
	UPROPERTY()
	float PendingHearingRange = 1200.f;

	// Visualization toggles
	// Read live by AAIDebugHUD::DrawHUD
	UPROPERTY()
	bool bShowPerceptionCones = true;

	UPROPERTY()
	bool bShowStateLabels = true;

	// Apply Pending* values to all live AEnemyAIControllers in World
	void ApplyToAllAI(UWorld* World);
};