#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

// Forward declarations to reduce compile dependencies
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UBehaviorTree;

/*
* AI Controller for enemy characters.
* Handles perception (sight, hearing, damage) and AI behavior logic.
*/
UCLASS()
class TACTICALAISHOWCASE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Setup perception components and default configurations
	AEnemyAIController(const FObjectInitializer& Init);

	/*
	* Exposes the perception component for external systems
	* Useful for debugging, behavior tree queries, or environment queries
	*/
	UAIPerceptionComponent* GetPerception() const { return PerceptionComp; }

	/*
	* Returns the team ID for this AI controller
	* Team 1 = enemy, Team 0 = player, Team 255 = neutral
	*/
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	// Called when this controller takes possession of a pawn (enemy character)
	virtual void OnPossess(APawn* InPawn) override;

	// Called when this controller loses possession of a pawn
	virtual void OnUnPossess() override;

	// Behavior tree to run on possess (assigned in BP_MyAIController Class Defaults)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// Core perception component that manages all AI senses
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;

	// Vision configuration (distance, FOV, detection rules)
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// Hearing configuration (noise detection radius, thresholds)
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	// Damage sense configuration (react to damage events)
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

	/*
	* Callback when AI perception system detects or updates a stimulus
	* @param Actor - Actor that triggered the stimulus
	* @param Stim - Stimulus data (type, strength, success state, etc.)
	*/
	UFUNCTION()
	void OnTargetSensed(AActor* Actor, FAIStimulus Stim);

private:
	/*
	* Applies configurable sense values(radius, FOV, age) to each sense config
	* Called during construction before senses are registered with the perception component
	* Safe to call only after SightConfig, HearingConfig, and DamageConfig are initialized
	*/
	void ApplyConfiguredSenseValues();
};
