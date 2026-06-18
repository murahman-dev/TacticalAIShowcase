#include "Slate/SAIDebugPanel.h"
#include "AI/AIDebugSettings.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"

// Three-column row used for each tuner field
// 45% label, 40% slider, 15% value marker
static TSharedRef<SHorizontalBox> Row(
	const FString& LabelStr,
	TSharedPtr<STextBlock>& OutValueLabel,
	float Min, float Max, float Value,
	FOnFloatValueChanged OnChanged)
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().FillWidth(0.45f).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString(LabelStr))
		]
		+ SHorizontalBox::Slot().FillWidth(0.4f).VAlign(VAlign_Center)
		[
			SNew(SSlider).MinValue(Min).MaxValue(Max).Value(Value).OnValueChanged(OnChanged)
		]
		+ SHorizontalBox::Slot().FillWidth(0.15f).VAlign(VAlign_Center)
		[
			SAssignNew(OutValueLabel, STextBlock).Text(FText::AsNumber(FMath::RoundToInt(Value)))
		];
}

void SAIDebugPanel::Construct(const FArguments& InArgs)
{
	DebugSettings = InArgs._Settings;
	World = InArgs._World;
	UAIDebugSettings* Settings = DebugSettings.Get();

	ChildSlot
		[
			SNew(SBorder)
				// Outer SBorder paints the dark translucent background tint
				// for better readability
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.85f))
				.Padding(FMargin(8.f))
				[
					// Inner SBorder gives the content uniform padding inside the tint
					SNew(SBorder).Padding(10.f)
						[
							SNew(SVerticalBox)

								+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f)
								[
									SNew(STextBlock).Text(FText::FromString(TEXT("AI Debug Panel")))
								]

								// Tuner sliders
								// OnValueChanged writes Pending* on drag
								// Value readout updated on drag
								+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
								[
									Row(TEXT("Sight Range"),
										SightLabel,
										200.f,
										4000.f,
										Settings ? Settings->PendingSightRange : 1500.f,
										FOnFloatValueChanged::CreateSP(this, &SAIDebugPanel::OnSightChanged))
								]
							+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
								[
									Row(TEXT("Lose Sight Range"),
										LoseSightLabel,
										300.f,
										6000.f,
										Settings ? Settings->PendingLoseSightRange : 2000.f,
										FOnFloatValueChanged::CreateSP(this, &SAIDebugPanel::OnLoseSightChanged))
								]
							+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
								[
									Row(TEXT("Half FOV Degree"),
										FOVLabel,
										5.f,
										90.f,
										Settings ? Settings->PendingHalfFOVDegree : 45.f,
										FOnFloatValueChanged::CreateSP(this, &SAIDebugPanel::OnFOVChanged))
								]
							+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
								[
									Row(TEXT("Hearing"),
										HearingLabel,
										100.f,
										3000.f,
										Settings ? Settings->PendingHearingRange : 1200.f,
										FOnFloatValueChanged::CreateSP(this, &SAIDebugPanel::OnHearingChanged))
								]

							// Visualization toggles
							// Write bShow* directly
							// Read live by DrawHUD
							+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
								[
									SNew(SCheckBox)
										.IsChecked(Settings && Settings->bShowPerceptionCones
											? ECheckBoxState::Checked
											: ECheckBoxState::Unchecked)
										.OnCheckStateChanged(this, &SAIDebugPanel::OnConesToggled)
										.Content()
										[
											SNew(STextBlock).Text(FText::FromString(TEXT("Show Cones")))
										]
								]
							+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
								[
									SNew(SCheckBox)
										.IsChecked(Settings && Settings->bShowStateLabels
											? ECheckBoxState::Checked
											: ECheckBoxState::Unchecked)
										.OnCheckStateChanged(this, &SAIDebugPanel::OnLabelsToggled)
										.Content()
										[
											SNew(STextBlock).Text(FText::FromString(TEXT("Show Labels")))
										]
								]

							// Apply button
							// Pushes Pending* values into live AEnemyAIControllers on button click
							+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 8.f)
								[
									SNew(SButton)
										.Text(FText::FromString(TEXT("Apply Values to All AI")))
										.HAlign(EHorizontalAlignment::HAlign_Center)
										.VAlign(EVerticalAlignment::VAlign_Center)
										.OnClicked(this, &SAIDebugPanel::OnApplyClicked)
								]
						]
				]
		];
}

void SAIDebugPanel::OnSightChanged(float V)
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		S->PendingSightRange = V;
	}
	if (SightLabel)
	{
		SightLabel->SetText(FText::AsNumber(FMath::RoundToInt(V)));
	}
}

void SAIDebugPanel::OnLoseSightChanged(float V)
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		S->PendingLoseSightRange = V;
	}
	if (LoseSightLabel)
	{
		LoseSightLabel->SetText(FText::AsNumber(FMath::RoundToInt(V)));
	}
}

void SAIDebugPanel::OnFOVChanged(float V)
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		S->PendingHalfFOVDegree = V;
	}
	if (FOVLabel)
	{
		FOVLabel->SetText(FText::AsNumber(FMath::RoundToInt(V)));
	}
}

void SAIDebugPanel::OnHearingChanged(float V)
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		S->PendingHearingRange = V;
	}
	if (HearingLabel)
	{
		HearingLabel->SetText(FText::AsNumber(FMath::RoundToInt(V)));
	}
}

void SAIDebugPanel::OnConesToggled(ECheckBoxState NewState)
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		S->bShowPerceptionCones = (NewState == ECheckBoxState::Checked);
	}
}

void SAIDebugPanel::OnLabelsToggled(ECheckBoxState NewState)
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		S->bShowStateLabels = (NewState == ECheckBoxState::Checked);
	}
}

FReply SAIDebugPanel::OnApplyClicked()
{
	if (UAIDebugSettings* S = DebugSettings.Get())
	{
		if (UWorld* W = World.Get())
		{
			S->ApplyToAllAI(W);
		}
	}
	return FReply::Handled();
}