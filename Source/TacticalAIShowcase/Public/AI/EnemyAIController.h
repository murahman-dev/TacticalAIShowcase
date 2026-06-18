#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIDebugSettings.h"
#include "GameplayTagContainer.h"
#include "EnemyAIController.generated.h"

// Forward declarations to reduce compile dependencies
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UBehaviorTree;

/*
* AI controller for enemy characters.
* Owns perception (sight/hearing/damage), writes stimuli into the blackboard,
* runs the assigned BT on possess, and registers with the squad subsystem.
* Team 1 in IGenericTeamAgentInterface.
*/
UCLASS()
class TACTICALAISHOWCASE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyAIController(const FObjectInitializer& Init);

	// Exposes the perception component for external systems (debug, BT, EQS contexts)
	UAIPerceptionComponent* GetPerception() const { return PerceptionComp; }

	// Applied at runtime when the tuner panel changes values
	void ApplyDebugSettings(const UAIDebugSettings& DebugSettings);

	// IGenericTeamAgentInterface team ID: 1 = enemy, 0 = player, 255 = neutral
	virtual FGenericTeamId GetGenericTeamId() const override;

	// Hostile for player-controlled pawns, friendly for other AI (perception affiliation filter)
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	// Role assigned to this AI on possess
	// Set per-subclass (Suppressor, Flanker, Closer)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Squad", meta = (Categories = "AI.Role"))
	FGameplayTag DefaultRole;

protected:
	// Called when this controller takes possession of a pawn (enemy character)
	virtual void OnPossess(APawn* InPawn) override;

	// Called when this controller loses possession of a pawn
	virtual void OnUnPossess() override;

	// Behavior tree to run on possess (set in the AIController BP class defaults)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// Perception component that manages all AI senses
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;

	// Vision configuration (distance, FOV, detection rules)
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// Hearing configuration (noise detection radius, max age)
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	// Damage sense configuration (max age for damage stimuli)
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	// Maximum distance at which AI can see targets
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "100", ClampMax = "6000"))
	float SightRadius = 1500.f;

	// Distance at which AI stops seeing a previously detected target
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "100", ClampMax = "8000"))
	float LoseSightRadius = 2000.f;

	// Half of the vision cone angle (in degrees)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "5", ClampMax = "90"))
	float PeripheralHalfAngleDeg = 45.f;

	// Radius within which AI can hear noises
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception", meta = (ClampMin = "100", ClampMax = "4000"))
	float HearingRadius = 1200.f;

	// Called when a sense stimulus fires
	// Updates BB target and last known location
	UFUNCTION()
	void OnTargetSensed(AActor* Actor, FAIStimulus Stim);

private:
	// Applies configurable sense values (radius, FOV, age) to each sense config
	// Called from the constructor before ConfigureSense
	// Configs must be allocated first
	void ApplyConfiguredSenseValues();
};
