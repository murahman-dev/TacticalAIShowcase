#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AIDebugHUD.generated.h"

class SAIDebugPanel;
class UAIDebugSettings;

/*
* Debug HUD.
* Hosts SAIDebugPanel (bottom-right viewport).
* AIDebugSettings owned via UPROPERTY (GC root for the tuner state).
* DrawHUD reads bShow* toggles live for state labels and perception cones.
* Tick enabled for the DrawHUD pass.
*/
UCLASS()
class TACTICALAISHOWCASE_API AAIDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAIDebugHUD();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DrawHUD() override;

protected:
	UPROPERTY()
	TObjectPtr<UAIDebugSettings> AIDebugSettings;

	TSharedPtr<SAIDebugPanel> DebugPanel;
	TSharedPtr<SWidget> DebugPanelContainer;
};