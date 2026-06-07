#include "AI/AITags.h"

/*
* Definitions for the AITags externs.
* Strings must match the tag names used in editor assets.
*/
namespace AITags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Patrol, "AI.State.Patrol");
	UE_DEFINE_GAMEPLAY_TAG(State_Alert, "AI.State.Alert");
	UE_DEFINE_GAMEPLAY_TAG(State_Engage, "AI.State.Engage");
	UE_DEFINE_GAMEPLAY_TAG(State_Flank, "AI.State.Flank");
	UE_DEFINE_GAMEPLAY_TAG(State_Retreat, "AI.State.Retreat");

	UE_DEFINE_GAMEPLAY_TAG(Role_Suppressor, "AI.Role.Supressor");
	UE_DEFINE_GAMEPLAY_TAG(Role_Flanker, "AI.Role.Flanker");
	UE_DEFINE_GAMEPLAY_TAG(Role_Closer, "AI.Role.Closer");
}
