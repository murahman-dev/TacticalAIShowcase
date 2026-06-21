#include "Save/TacticalAISaveGame.h"

void UTacticalAISaveGame::MigrateIfNeeded()
{
	if (SchemaVersion == TACTICAL_AI_SAVE_VERSION)
	{
		return;
	}

	// Add vN -> vN+1 blocks above this ensure, oldest first
	ensureMsgf(false, TEXT("Unknown Save Schema %d"), SchemaVersion);
}