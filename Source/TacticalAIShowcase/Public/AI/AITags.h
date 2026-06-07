#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/*
* Compile-checked gameplay tags for AI state and squad role.
* Native tags declared here, defined in AITags.cpp.
* Registered with the GameplayTagsManager at module startup.
*/
namespace AITags
{
	// State tags: which behavior the AI is currently running
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Patrol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Alert);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Engage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Flank);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Retreat);

	// Role tags: tactical role within a group of AI
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Suppressor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Flanker);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Closer);
}