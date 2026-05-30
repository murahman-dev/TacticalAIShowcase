#include "TacticalAIGameInstance.h"
#include "GenericTeamAgentInterface.h"

UTacticalAIGameInstance::UTacticalAIGameInstance() {}

void UTacticalAIGameInstance::Init()
{
	Super::Init();

	// Register the global team attitude solver.
	// Same team -> Friendly, team 255 -> Neutral, otherwise Hostile.
	FGenericTeamId::SetAttitudeSolver(
		[](FGenericTeamId A, FGenericTeamId B) -> ETeamAttitude::Type
		{
			if (A == B) return ETeamAttitude::Friendly;

			// Team 255 is the FGenericTeamId sentinel for unassigned teams
			if (A.GetId() == 255 || B.GetId() == 255) return ETeamAttitude::Neutral;

			return ETeamAttitude::Hostile;
		});
}

void UTacticalAIGameInstance::Shutdown()
{
	Super::Shutdown();
}