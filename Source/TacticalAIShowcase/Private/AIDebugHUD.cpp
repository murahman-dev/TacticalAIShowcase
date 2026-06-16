#include "AIDebugHUD.h"
#include "AI/AIDebugSettings.h"
#include "AI/EnemyAIController.h"
#include "Slate/SAIDebugPanel.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BlackboardKeys.h"

namespace
{
	// Height above pawn root for state label (cm)
	// Above default capsule (~96 cm)
	constexpr float StateLabelHeightCm = 110.f;

	// Tuner panel size (px)
	constexpr float PanelWidthPx = 300.f;
	constexpr float PanelHeightPx = 240.f;
}

AAIDebugHUD::AAIDebugHUD()
{
	// Tick enabled for the DrawHUD pass
	PrimaryActorTick.bCanEverTick = true;
}

void AAIDebugHUD::BeginPlay()
{
	Super::BeginPlay();

	// HUD owns the settings via UPROPERTY
	// (since SAIDebugPanel only holds a weak ref)
	AIDebugSettings = NewObject<UAIDebugSettings>(this);

	// Outer SBox anchors the panel bottom-right with 20 px edge padding
	// Inner SBox fixes the panel size
	DebugPanelContainer = SNew(SBox)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.f, 0.f, 20.f, 20.f))
		[
			SNew(SBox)
				.WidthOverride(PanelWidthPx)
				.HeightOverride(PanelHeightPx)
				[
					SAssignNew(DebugPanel, SAIDebugPanel)
						.Settings(AIDebugSettings)
						.World(GetWorld())
				]
		];

	APlayerController* PC = GetOwningPlayerController();
	if (GEngine && GEngine->GameViewport && PC && PC->GetLocalPlayer())
	{
		// z-order 10
		// Above gameplay HUD, below dev/editor overlays
		GEngine->GameViewport->AddViewportWidgetForPlayer(
			PC->GetLocalPlayer(),
			DebugPanelContainer.ToSharedRef(),
			10);
	}
}

void AAIDebugHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayerController* PC = GetOwningPlayerController();

	// Null pointer Guard for
	// no valid local player or proper world context
	if (DebugPanelContainer.IsValid() && GEngine && GEngine->GameViewport && PC && PC->GetLocalPlayer())
	{
		GEngine->GameViewport->RemoveViewportWidgetForPlayer(
			PC->GetLocalPlayer(),
			DebugPanelContainer.ToSharedRef());
	}
	Super::EndPlay(EndPlayReason);
}

void AAIDebugHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!AIDebugSettings)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

#if !UE_BUILD_SHIPPING
	for (TActorIterator<AEnemyAIController> It(World); It; ++It)
	{
		AEnemyAIController* AI_Controller = *It;
		APawn* AI_Pawn = AI_Controller ? AI_Controller->GetPawn() : nullptr;
		UBlackboardComponent* BB = AI_Controller ? AI_Controller->GetBlackboardComponent() : nullptr;

		if (!AI_Pawn || !BB)
		{
			continue;
		}

		const FVector Location = AI_Pawn->GetActorLocation() + FVector(0.f, 0.f, StateLabelHeightCm);

		// State label
		// Draws CurrentState BB key above the pawn
		if (AIDebugSettings->bShowStateLabels)
		{
			const FName State = BB->GetValueAsName(BBKeys::CurrentState);
			DrawDebugString(World, Location, State.ToString(), nullptr, FColor::Yellow, 0.f, true, 1.2f);
		}

		// Perception cone
		// Slider drag updates cone immediately
		// No Apply needed for preview
		if (AIDebugSettings->bShowPerceptionCones)
		{
			if (AI_Controller->GetPerceptionComponent())
			{
				const FVector Apex = AI_Pawn->GetActorLocation();
				const FVector Forward = AI_Pawn->GetActorForwardVector();
				const float HalfFOVRad = FMath::DegreesToRadians(AIDebugSettings->PendingHalfFOVDegree);
				DrawDebugCone(World, Apex, Forward, AIDebugSettings->PendingSightRange, HalfFOVRad, HalfFOVRad, 16, FColor::Cyan, false, 0.f, 0, 0.6f);
			}
		}
	}
#endif
}