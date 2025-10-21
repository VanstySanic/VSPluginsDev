// Copyright VanstySanic. All Rights Reserved.

#include "Interactive/Feature/VSInteractiveFeature.h"
#include "VSInteractSystemUtils.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Interactive/Feature/Action/VSInteractiveActionFeature.h"

UVSInteractiveFeature::UVSInteractiveFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInteractiveFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	InteractiveFeatureAgentPrivate = Cast<UVSInteractiveFeatureAgent>(this);
	if (!InteractiveFeatureAgentPrivate.IsValid()) { InteractiveFeatureAgentPrivate = FindOwnerFeatureByClass<UVSInteractiveFeatureAgent>(); }
	if (!InteractiveFeatureAgentPrivate.IsValid()) { InteractiveFeatureAgentPrivate = UVSInteractSystemUtils::GetInteractiveFeatureAgentFromActor(GetOwnerActor()); }
	check(InteractiveFeatureAgentPrivate.IsValid());
}

bool UVSInteractiveFeature::IsInspectedBySource(UVSInteractFeatureAgent* SourceAgent) const
{
	if (!SourceAgent || !InteractiveFeatureAgentPrivate.IsValid()) return false;
	return InteractiveFeatureAgentPrivate->InspectingSources.Contains(SourceAgent);
}

bool UVSInteractiveFeature::IsInteractedBySource(UVSInteractFeatureAgent* SourceAgent) const
{
	if (!SourceAgent || !InteractiveFeatureAgentPrivate.IsValid()) return false;
	return InteractiveFeatureAgentPrivate->InteractingSourceActionMap.Contains(SourceAgent);
}

float UVSInteractiveFeature::GetInteractionTime(UVSInteractFeatureAgent* SourceAgent) const
{
	if (!SourceAgent || !InteractiveFeatureAgentPrivate.IsValid() || !IsInteractedBySource(SourceAgent)) return false;
	FName ActionFeatureName = InteractiveFeatureAgentPrivate->InteractingSourceActionMap.FindRef(SourceAgent);
	if (UVSInteractiveActionFeature* ActionFeature = InteractiveFeatureAgentPrivate->GetActionFeatureByName(ActionFeatureName))
	{
		return ActionFeature->GetActionInteractedTime(SourceAgent);
	}
	return 0.f;	
}
