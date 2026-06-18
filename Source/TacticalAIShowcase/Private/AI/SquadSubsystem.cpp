#include "AI/SquadSubsystem.h"
#include "AI/AITags.h"
#include "GameFramework/Pawn.h"

void USquadSubsystem::RegisterAlly(APawn* Pawn, FGameplayTag Role)
{
	if (!Pawn)
	{
		return;
	}

	// Drop any prior entry for this pawn 
	// So the Add below doesn't create a duplicate
	Allies.RemoveAll([Pawn](const FAllyInfo& A)
		{
			return A.Pawn.Get() == Pawn;
		}
	);

	FAllyInfo A;
	A.Pawn = Pawn;
	A.Role = Role;
	A.State = AITags::State_Patrol;
	Allies.Add(MoveTemp(A));
}

void USquadSubsystem::UnregisterAlly(APawn* Pawn)
{
	if (!Pawn)
	{
		return;
	}

	Allies.RemoveAll([Pawn](const FAllyInfo& A)
		{
			return A.Pawn.Get() == Pawn;
		}
	);
}

void USquadSubsystem::SetState(APawn* Pawn, FGameplayTag StateTag)
{
	for (FAllyInfo& A : Allies)
	{
		if (A.Pawn.Get() == Pawn)
		{
			A.State = StateTag;
			break;
		}
	}
}

void USquadSubsystem::SetTarget(APawn* Pawn, AActor* Target)
{
	for (FAllyInfo& A : Allies)
	{
		if (A.Pawn.Get() == Pawn)
		{
			A.Target = Target;
			break;
		}
	}
}

int32 USquadSubsystem::NumberOfEngagedAllies(APawn* Asker) const
{
	int32 N = 0;
	for (const FAllyInfo& A : Allies)
	{
		// Exclude self
		if (A.Pawn.Get() == Asker)
		{
			continue;
		}

		// Skip stale TWeakObjectPtr (pawn destroyed)
		if (!A.Pawn.IsValid())
		{
			continue;
		}

		if (A.State == AITags::State_Engage || A.State == AITags::State_Flank)
		{
			++N;
		}
	}
	return N;
}

AActor* USquadSubsystem::MostEngagedTarget(APawn* Asker) const
{
	for (const FAllyInfo& A : Allies)
	{
		// Exclude self
		if (A.Pawn.Get() == Asker)
		{
			continue;
		}

		// Skip stale TWeakObjectPtr (pawn destroyed)
		if (!A.Pawn.IsValid())
		{
			continue;
		}

		if (A.State == AITags::State_Engage && A.Target.IsValid())
		{
			return A.Target.Get();
		}
	}
	return nullptr;
}