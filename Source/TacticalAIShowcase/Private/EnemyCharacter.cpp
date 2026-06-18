#include "EnemyCharacter.h"
#include "AI/EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	// Disable ticking unless explicitly needed (AI is event-driven)
	PrimaryActorTick.bCanEverTick = false;

	// Auto-possess on spawn or level placement
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Custom controller owns perception, blackboard wiring, and team affiliation
	AIControllerClass = AEnemyAIController::StaticClass();
}

// Returns actor rotation directly instead of controller rotation
// Perception uses this for sight, to fix cone-lagging-behind-body when
// AAIController::Tick (which drives UpdateControlRotation) is disabled
void AEnemyCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetActorLocation() + FVector(0.0f, 0.0f, BaseEyeHeight);
	OutRotation = GetActorRotation();
}