#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SSlider;
class STextBlock;
class UAIDebugSettings;

/*
* Slate tuning panel for AI perception values.
* Built by AAIDebugHUD, anchored to the bottom-right viewport corner.
* Slider drag writes UAIDebugSettings::Pending* variables live.
* ApplyToAllAI applies them on live AEnemyAIControllers on button click.
* Settings/World held weakly so panel survives if either UObject is destroyed mid-session.
*/
class TACTICALAISHOWCASE_API SAIDebugPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAIDebugPanel) {}
		SLATE_ARGUMENT(TWeakObjectPtr<UAIDebugSettings>, Settings)
		SLATE_ARGUMENT(TWeakObjectPtr<UWorld>, World)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply OnApplyClicked();

	void OnSightChanged(float V);
	void OnLoseSightChanged(float V);
	void OnFOVChanged(float V);
	void OnHearingChanged(float V);
	void OnConesToggled(ECheckBoxState NewState);
	void OnLabelsToggled(ECheckBoxState NewState);

	TWeakObjectPtr<UAIDebugSettings> DebugSettings;
	TWeakObjectPtr<UWorld> World;
	TSharedPtr<STextBlock> SightLabel;
	TSharedPtr<STextBlock> LoseSightLabel;
	TSharedPtr<STextBlock> FOVLabel;
	TSharedPtr<STextBlock> HearingLabel;
};