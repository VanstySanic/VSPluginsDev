// Copyright VanstySanic. All Rights Reserved.

#include "Interactive/Feature/Action/VSInteractiveActionFeature.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"

UVSInteractiveActionFeature::UVSInteractiveActionFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BreakInteractionSourceTagQueries.bQueriesEmptyAsPass = false;
}

bool UVSInteractiveActionFeature::IsAvailableForSource_Implementation(UVSInteractFeatureAgent* SourceAgent) const
{
	if (!bAllowInteraction || !SourceAgent || SourceAgent->IsInteracting()) return false;

	if (RemainedInteractionCoolDown > 0.f || InteractingSourceRemainedCoolDownMap.FindRef(SourceAgent) > 0.f) return false;
	if (MaxInteractionCount > 0 && CurrentInteractionCount >= MaxInteractionCount) return false;
	if (MaxInteractionCountForSource > 0 && SourceInteractionCountMap.FindRef(SourceAgent) >= MaxInteractionCountForSource) return false;
	if (MaxSimultaneousInteractionNum > 0 && InteractingSourceTimeMap.Num() >= MaxSimultaneousInteractionNum) return false;
	
	FGameplayTagContainer SourceTags = SourceAgent->GetGameplayTagController()->GetGameplayTags();
	if (!AvailableEntrySourceTagQuery.IsEmpty() && !AvailableEntrySourceTagQuery.Matches(SourceTags)) return false;
	if (AvailableBlockSourceTagQuery.Matches(SourceTags)) return false;
	
	return true;
}

void UVSInteractiveActionFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	RemainedInteractionCoolDown = FMath::Clamp(RemainedInteractionCoolDown - DeltaTime, 0.f, InteractionCoolDown);
	TArray<TWeakObjectPtr<UVSInteractFeatureAgent>> CoolDownSourcesToRemove;
	for (auto& SourceRemainedCoolDownMap : InteractingSourceRemainedCoolDownMap)
	{
		SourceRemainedCoolDownMap.Value = FMath::Clamp(SourceRemainedCoolDownMap.Value - DeltaTime, 0.f, InteractionCoolDown);
		if (FMath::IsNearlyZero(SourceRemainedCoolDownMap.Value))
		{
			CoolDownSourcesToRemove.Add(SourceRemainedCoolDownMap.Key);
		}
	}
	for (TWeakObjectPtr<UVSInteractFeatureAgent> InteractFeatureAgent : CoolDownSourcesToRemove)
	{
		InteractingSourceRemainedCoolDownMap.Remove(InteractFeatureAgent);
	}

	TArray<TWeakObjectPtr<UVSInteractFeatureAgent>> InteractionSourcesToEnd;
	TMap<TWeakObjectPtr<UVSInteractFeatureAgent>, float> CopiedInteractingSourceTimeMap = InteractingSourceTimeMap;
	for (auto& CopiedSourceTimeMap : CopiedInteractingSourceTimeMap)
	{
		auto& InteractionTime = InteractingSourceTimeMap[CopiedSourceTimeMap.Key];
		const bool bHasDuration = !bIsInstantAction && InteractionDuration > 0.f;

		InteractionTime += DeltaTime;
		if (bHasDuration)
		{
			InteractionTime = FMath::Clamp(InteractionTime, 0.f, InteractionDuration);
		}
		OnProcessInteraction(CopiedSourceTimeMap.Key.Get(), DeltaTime);
		if (bHasDuration && InteractionTime >= InteractionDuration)
		{
			InteractionSourcesToEnd.Add(CopiedSourceTimeMap.Key.Get());
		}
	}
	for (TWeakObjectPtr<UVSInteractFeatureAgent> InteractFeatureAgent : InteractionSourcesToEnd)
	{
		EndInteraction(InteractFeatureAgent.Get(), true);
	}
}

void UVSInteractiveActionFeature::EndInteraction(UVSInteractFeatureAgent* SourceAgent, bool bIsLocalBreak)
{
	if (!SourceAgent || !SourceAgent->IsInteracting() || SourceAgent->GetCurrentInteractiveActionFeature() != this) return;
	SourceAgent->StopInteractionWithTarget(GetInteractiveFeatureAgent(), FeatureName, bIsLocalBreak);
	if (!bCoolDownAtInteractionStart)
	{
		RemainedInteractionCoolDown = InteractionCoolDown;
		InteractingSourceRemainedCoolDownMap.Emplace(SourceAgent, InteractionCoolDown);
	}
}

void UVSInteractiveActionFeature::OnStartInteraction_Implementation(UVSInteractFeatureAgent* SourceAgent)
{
	CurrentInteractionCount++;
	SourceInteractionCountMap.Emplace(SourceAgent, SourceInteractionCountMap.FindRef(SourceAgent) + 1);
	InteractingSourceTimeMap.Emplace(SourceAgent, 0.f);
	if (bCoolDownAtInteractionStart)
	{
		RemainedInteractionCoolDown = InteractionCoolDown;
		InteractingSourceRemainedCoolDownMap.Emplace(SourceAgent, InteractionCoolDown);
	}
}

void UVSInteractiveActionFeature::OnProcessInteraction_Implementation(UVSInteractFeatureAgent* SourceAgent, const float DeltaTime)
{

}

void UVSInteractiveActionFeature::OnStopInteraction_Implementation(UVSInteractFeatureAgent* SourceAgent)
{
	InteractingSourceTimeMap.Remove(SourceAgent);
}

