#include "AI/AIDebugSettings.h"
#include "EngineUtils.h"
#include "AI/EnemyAIController.h"

void UAIDebugSettings::ApplyToAllAI(UWorld* World)
{
	if (!World)
	{
		return;
	}

	for (TActorIterator<AEnemyAIController> It(World); It; ++It)
	{
		AEnemyAIController* AI_Controller = *It;

		// Skip controllers missing perception component
		if (!AI_Controller || !AI_Controller->GetPerception())
		{
			continue;
		}

		AI_Controller->ApplyDebugSettings(*this);
	}
}