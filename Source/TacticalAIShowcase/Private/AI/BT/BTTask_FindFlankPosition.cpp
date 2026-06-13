#include "AI/BT/BTTask_FindFlankPosition.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQuery.h"

UBTTask_FindFlankPosition::UBTTask_FindFlankPosition()
{
	NodeName = TEXT("Find Flank Position");

	// Each AI gets its own task instance so CachedOwner is per-AI
	// Without this, the task is a shared CDO and CachedOwner
	// is shared across enemies
	bCreateNodeInstance = true;

	// Editor dropdown for ResultKey shows only Vector keys
	ResultKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFlankPosition, ResultKey));
}

EBTNodeResult::Type UBTTask_FindFlankPosition::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory)
{
	AAIController* AI_Controller = Owner.GetAIOwner();
	if (!AI_Controller || !Query)
	{
		return EBTNodeResult::Failed;
	}

	// Querier = pawn for spatial context and BB access via custom contexts
	FEnvQueryRequest Req(Query, AI_Controller->GetPawn());

	// Cache owner for the async callback
	CachedOwner = &Owner;

	// SingleResult: best-scored item from the query
	Req.Execute(EEnvQueryRunMode::SingleResult, this, &UBTTask_FindFlankPosition::OnQueryDone);

	// Latent: OnQueryDone finishes the task
	return EBTNodeResult::InProgress;
}

void UBTTask_FindFlankPosition::OnQueryDone(TSharedPtr<FEnvQueryResult> Result)
{
	UBehaviorTreeComponent* Owner = CachedOwner.Get();

	if (!Owner)
	{
		// BT destroyed between Execute and callback
		return;
	}

	if (!Result.IsValid() || !Result->IsSuccessful())
	{
		FinishLatentTask(*Owner, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = Owner->GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(*Owner, EBTNodeResult::Failed);
		return;
	}
	BB->SetValueAsVector(ResultKey.SelectedKeyName, Result->GetItemAsLocation(0));
	FinishLatentTask(*Owner, EBTNodeResult::Succeeded);
}
