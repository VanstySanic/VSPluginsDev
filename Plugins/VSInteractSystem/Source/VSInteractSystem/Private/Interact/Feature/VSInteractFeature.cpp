// Copyright VanstySanic. All Rights Reserved.

#include "Interact/Feature/VSInteractFeature.h"
#include "VSInteractSystemUtils.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"

UVSInteractFeature::UVSInteractFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInteractFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	InteractFeatureAgentPrivate = Cast<UVSInteractFeatureAgent>(this);
	if (!InteractFeatureAgentPrivate.IsValid())
	{
		InteractFeatureAgentPrivate = UVSInteractSystemUtils::GetInteractFeatureAgentFromActor(GetOwnerActor());
	}
	check(InteractFeatureAgentPrivate.IsValid());
}

UVSGameplayTagController* UVSInteractFeature::GetGameplayTagController() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->GameplayTagControllerPrivate.Get() : nullptr;
}

UVSInteractiveFeatureAgent* UVSInteractFeature::GetCurrentInspectiveFeatureAgent() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->CurrentInspectivePrivate.Get() : nullptr;
}

UVSInteractiveFeatureAgent* UVSInteractFeature::GetCurrentInteractiveFeatureAgent() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->CurrentInteractivePrivate.Get() : nullptr;
}

FName UVSInteractFeature::GetCurrentInteractiveActionFeatureName() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->CurrentInteractiveActionFeatureName : NAME_None;
}

UVSInteractiveActionFeature* UVSInteractFeature::GetCurrentInteractiveActionFeature() const
{
	if (!IsInteracting()) return nullptr;
	return InteractFeatureAgentPrivate->CurrentInteractivePrivate->GetActionFeatureByName(InteractFeatureAgentPrivate->CurrentInteractiveActionFeatureName);
}

bool UVSInteractFeature::IsInspecting() const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return false;
	if (!InteractFeatureAgentPrivate->CurrentInspectivePrivate.IsValid()) return false;
	return InteractFeatureAgentPrivate->CurrentInspectivePrivate->IsInspectedBySource(GetInteractFeatureAgent());
}

bool UVSInteractFeature::IsInteracting() const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return false;
	if (!InteractFeatureAgentPrivate->CurrentInteractivePrivate.IsValid()) return false;
	return InteractFeatureAgentPrivate->CurrentInteractivePrivate->IsInteractedBySource(GetInteractFeatureAgent());
}
