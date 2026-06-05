#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace AITags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Patrol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Alert);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Engage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Flank);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Retreat);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Suppressor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Flanker);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Closer);
}