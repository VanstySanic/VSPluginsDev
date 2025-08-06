// Copyright VanstySanic. All Rights Reserved.

#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/Action/VSInteractiveActionFeature.h"

UVSInteractiveFeatureAgent::UVSInteractiveFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSInteractiveFeatureAgent::Initialize_Implementation()
{
	Super::Super::Initialize_Implementation();

	InteractiveFeatureAgentPrivate = this;
}


void UVSInteractiveFeatureAgent::EndPlay_Implementation()
{
	const TArray<TWeakObjectPtr<UVSInteractFeatureAgent>> CopiedInspectingSources = InspectingSources;
	for (TWeakObjectPtr<UVSInteractFeatureAgent> InteractFeatureAgent : CopiedInspectingSources)
	{
		if (InteractFeatureAgent.IsValid() && InteractFeatureAgent->IsInspecting() && InteractFeatureAgent->GetCurrentInspectiveFeatureAgent() == this)
		{
			InteractFeatureAgent->StopInspectionOnTarget(this);
		}
	}

	const TMap<TWeakObjectPtr<UVSInteractFeatureAgent>, FName> CopiedInteractingSourceActionMap = InteractingSourceActionMap;
	for (const auto& SourceActionMap : CopiedInteractingSourceActionMap)
	{
		if (SourceActionMap.Key.IsValid() && SourceActionMap.Value.IsValid())
		{
			SourceActionMap.Key.Get()->StopInteractionWithTarget(this, SourceActionMap.Value, true);
		}
	}
	
	Super::EndPlay_Implementation();
}

UVSInteractiveActionFeature* UVSInteractiveFeatureAgent::GetActionFeatureByName(const FName ActionName) const
{
	return Cast<UVSInteractiveActionFeature>(GetSubFeatureByName(ActionName));
}

TArray<UVSInteractiveActionFeature*> UVSInteractiveFeatureAgent::GetAvailableActionFeatures(UVSInteractFeatureAgent* SourceAgent) const
{
	TArray<UVSInteractiveActionFeature*> AvailableActionFeatures;
	TArray<UVSInteractiveActionFeature*> ActionFeatures = FindSubFeaturesByClass<UVSInteractiveActionFeature>();
	for (UVSInteractiveActionFeature* ActionFeature : ActionFeatures)
	{
		if (ActionFeature && ActionFeature->IsAvailableForSource(SourceAgent))
		{
			AvailableActionFeatures.Add(ActionFeature);
		}
	}
	return AvailableActionFeatures;
}

bool UVSInteractiveFeatureAgent::IsInspectable(UVSInteractFeatureAgent* SourceAgent) const
{
	if (!bAllowInspection || !SourceAgent || SourceAgent->IsInteracting()) return false;
	if (InspectRequireInteractivity && !IsInteractable(SourceAgent)) return false;
	return true;
}

bool UVSInteractiveFeatureAgent::IsInteractable(UVSInteractFeatureAgent* SourceAgent) const
{
	if (!bAllowInteraction || !SourceAgent || GetAvailableActionFeatures(SourceAgent).IsEmpty()) return false;
	FGameplayTagContainer SourceTags = SourceAgent->GetGameplayTagController()->GetGameplayTags();
	if (!InteractionEntrySourceTagQuery.IsEmpty() && !InteractionEntrySourceTagQuery.Matches(SourceTags)) return false;
	if (InteractionBlockSourceTagQuery.Matches(SourceTags)) return false;
	return true;
}

bool UVSInteractiveFeatureAgent::IsActionAvailable(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName) const
{
	UVSInteractiveActionFeature* ActionFeature = GetActionFeatureByName(ActionFeatureName);
	return ActionFeature && ActionFeature->IsAvailableForSource(SourceAgent);
}

void UVSInteractiveFeatureAgent::OnInspectBySource(UVSInteractFeatureAgent* SourceAgent)
{
	InspectingSources.Emplace(SourceAgent);
	OnInspectionStart.Broadcast(SourceAgent);
}

void UVSInteractiveFeatureAgent::OnStopInspectBySource(UVSInteractFeatureAgent* SourceAgent)
{
	InspectingSources.RemoveSingle(SourceAgent);
	OnInspectionEnd.Broadcast(SourceAgent);
}

void UVSInteractiveFeatureAgent::OnInteractBySource(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName)
{
	UVSInteractiveActionFeature* ActionFeature = GetActionFeatureByName(ActionFeatureName);
	InteractingSourceActionMap.Emplace(SourceAgent, ActionFeatureName);
	ActionFeature->OnStartInteraction(SourceAgent);
	OnInteractionStart.Broadcast(SourceAgent, ActionFeatureName);

	if (ActionFeature->bIsInstantAction)
	{
		ActionFeature->OnProcessInteraction(SourceAgent, 0.f);
	}
}

void UVSInteractiveFeatureAgent::OnStopInteractionBySource(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName)
{
	UVSInteractiveActionFeature* ActionFeature = GetActionFeatureByName(ActionFeatureName);
	InteractingSourceActionMap.Remove(SourceAgent);
	ActionFeature->OnStopInteraction(SourceAgent);
	OnInteractionEnd.Broadcast(SourceAgent, ActionFeatureName);
}
