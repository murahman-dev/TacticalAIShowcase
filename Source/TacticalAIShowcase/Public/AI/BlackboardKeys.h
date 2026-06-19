#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"

/*
* Compile-checked blackboard key names.
* Replaces string literals at call sites so typos surface at compile time.
* Definitions in BlackboardKeys.cpp must match the key names authored in BB_Enemy exactly.
*/
namespace BBKeys
{
	// Target actor reference, set on sight stimulus and held across loss (Object key)
	extern const FName TargetActor;

	// Last position target was seen or heard at (Vector key)
	extern const FName LastKnownLocation;

	// True while sight stimulus is active for TargetActor (Bool key)
	extern const FName bSeesTarget;

	// Alert state has completed its investigate move (Bool key)
	extern const FName bHasInvestigated;

	// Patrol origin, seeded on possess from the pawn's spawn location (Vector key)
	extern const FName HomeLocation;

	// Squad role tag: Suppressor, Flanker, Closer (Name key)
	extern const FName Role;

	// Current state tag: Patrol, Alert, Engage, Flank, Retreat (Name key)
	extern const FName CurrentState;

	// Patrol destination written by BTTask_PickPatrolPoint (Vector key)
	extern const FName PatrolDestination;

	// Index for PatrolWaypoints, advanced per cycle with wraparound (Int key)
	extern const FName CurrentPatrolIndex;

	// Nav-valid flank position from EQS via BTTask_FindFlankPosition (Vector key)
	extern const FName FlankPosition;

	// Pawn's health is below the retreat threshold (Bool key)
	extern const FName LowHealth;
}
