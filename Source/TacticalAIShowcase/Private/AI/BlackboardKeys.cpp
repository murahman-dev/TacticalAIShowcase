#include "AI/BlackboardKeys.h"

/*
* Definitions for the BBKeys externs.
* Strings must match the BB_Enemy key spelling exactly.
*/
namespace BBKeys
{
	const FName TargetActor = TEXT("TargetActor");
	const FName LastKnownLocation = TEXT("LastKnownLocation");
	const FName bSeesTarget = TEXT("bSeesTarget");
	const FName bHasInvestigated = TEXT("bHasInvestigated");
	const FName HomeLocation = TEXT("HomeLocation");
	const FName Role = TEXT("Role");
	const FName CurrentState = TEXT("CurrentState");
	const FName PatrolDestination = TEXT("PatrolDestination");
	const FName CurrentPatrolIndex = TEXT("CurrentPatrolIndex");
	const FName FlankPosition = TEXT("FlankPosition");
	const FName LowHealth = TEXT("LowHealth");
}
