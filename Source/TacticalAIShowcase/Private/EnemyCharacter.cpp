#include "EnemyCharacter.h"
#include "AI/EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	// Disable ticking unless explicitly needed (AI is event-driven)
	PrimaryActorTick.bCanEverTick = false;

	// Ensure AI controller is automatically assigned when spawned or placed
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Set custom AI controller class
	AIControllerClass = AEnemyAIController::StaticClass();
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();	
}