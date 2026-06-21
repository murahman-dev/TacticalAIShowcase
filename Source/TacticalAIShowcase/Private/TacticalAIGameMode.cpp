#include "TacticalAIGameMode.h"
#include "PlayerCharacter.h"
#include "Save/TacticalAISaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AI/EnemyAIController.h"
#include "AI/BlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"

namespace TacticalAISaveSlots
{
	const FString Default = TEXT("TacticalAISaveSlot_Default");
}

ATacticalAIGameMode::ATacticalAIGameMode() {}

void ATacticalAIGameMode::SaveEncounter(const FString& Slot)
{
	// CreateSaveGameObject wraps NewObject for the save class registry
	UTacticalAISaveGame* Save = Cast<UTacticalAISaveGame>(UGameplayStatics::CreateSaveGameObject(UTacticalAISaveGame::StaticClass()));

	if (!Save)
	{
		return;
	}

	// Schema version stamps the save for MigrateIfNeeded on load
	Save->SchemaVersion = TACTICAL_AI_SAVE_VERSION;

	// UTC time to keep save timestamps unambiguous across machines
	Save->SavedAtUTC = FDateTime::UtcNow();

	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		Save->PlayerLocation = Pawn->GetActorLocation();
	}

	// TActorIterator<AEnemyAIController> walks all live actors of type
	// EnemyAIController in world
	for (TActorIterator<AEnemyAIController> It(GetWorld()); It; ++It)
	{
		APawn* AI_Pawn = (*It)->GetPawn();
		UBlackboardComponent* BB = (*It)->GetBlackboardComponent();

		// Skip controllers if pawn or BB missing
		if (!AI_Pawn || !BB)
		{
			continue;
		}

		FEnemySnapshot S;
		S.Location = AI_Pawn->GetActorLocation();
		S.Role = BB->GetValueAsName(BBKeys::Role);
		S.StateTag = BB->GetValueAsName(BBKeys::CurrentState);

		Save->Enemies.Add(MoveTemp(S));
	}

	UGameplayStatics::SaveGameToSlot(Save, Slot, 0);
}

void ATacticalAIGameMode::LoadEncounter(const FString& Slot)
{
	// Existence check before loading from a slot
	if (!UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		return;
	}

	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(Slot, 0);
	UTacticalAISaveGame* Save = Cast<UTacticalAISaveGame>(Loaded);

	// Cast guards against the save class being renamed
	// or replaced between sessions
	if (!Save)
	{
		return;
	}

	// Bring old-schema saves up to current before reading fields
	Save->MigrateIfNeeded();

	// Restore player position
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		Pawn->SetActorLocation(Save->PlayerLocation);
	}

	// Restore enemy positions by TActorIterator order
	// Fragile across level edits, logs a warning on count mismatch
	// Rotation, State, and Health re-evaluate from BT on next tick (not restored)
	const int32 SaveCount = Save->Enemies.Num();
	int32 LiveCount = 0;

	for (TActorIterator<AEnemyAIController> It(GetWorld()); It; ++It)
	{
		if (LiveCount < SaveCount)
		{
			AEnemyAIController* AI = *It;
			if (APawn* AI_Pawn = AI ? AI->GetPawn() : nullptr)
			{
				AI_Pawn->SetActorLocation(Save->Enemies[LiveCount].Location);
			}
		}
		++LiveCount;
	}

	if (LiveCount != SaveCount)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("LoadEncounter: %d saved snapshots vs %d live AI; restored %d positions"),
			SaveCount, LiveCount, FMath::Min(SaveCount, LiveCount));
	}
}