// Copyright VanstySanic. All Rights Reserved.

#include "Interact/Feature/VSInteractFeature.h"
#include "VSInteractSystemUtils.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Interactive/Feature/Action/VSInteractiveActionFeature.h"

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

	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSInteractFeature::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSInteractFeature::OnMovementTagEventNotified);
}

void UVSInteractFeature::Uninitialize_Implementation()
{
	if (UVSGameplayTagController* GameplayTagController = GetGameplayTagController())
	{
		GameplayTagController->OnTagsUpdated.RemoveDynamic(this, &UVSInteractFeature::OnMovementTagsUpdated);
		GameplayTagController->OnTagEventNotified.RemoveDynamic(this, &UVSInteractFeature::OnMovementTagEventNotified);
	}
	
	Super::Uninitialize_Implementation();
}

UVSGameplayTagController* UVSInteractFeature::GetGameplayTagController() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->GameplayTagControllerPrivate.Get() : nullptr;
}

TArray<UVSInteractiveFeatureAgent*> UVSInteractFeature::GetCurrentInspectiveFeatureAgents() const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return TArray<UVSInteractiveFeatureAgent*>();
	TArray<UVSInteractiveFeatureAgent*> Ans;
	for (TWeakObjectPtr<UVSInteractiveFeatureAgent> CurrentInspectiveAgentsPrivate : InteractFeatureAgentPrivate->CurrentInspectiveAgentsPrivate)
	{
		if (CurrentInspectiveAgentsPrivate.IsValid())
		{
			Ans.Emplace(CurrentInspectiveAgentsPrivate.Get());
		}
	}
	return Ans;
}

UVSInteractiveFeatureAgent* UVSInteractFeature::GetClosestInspectiveFeatureAgent() const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return nullptr;
	float ClosestDistance = 65536.f;
	UVSInteractiveFeatureAgent* Interactive = nullptr;
	for (TWeakObjectPtr<UVSInteractiveFeatureAgent> CurrentInspectiveAgentsPrivate : InteractFeatureAgentPrivate->CurrentInspectiveAgentsPrivate)
	{
		if (!CurrentInspectiveAgentsPrivate.IsValid()) continue;
		const float Distance = (CurrentInspectiveAgentsPrivate->GetOwnerActor()->GetActorLocation() - GetOwnerActor()->GetActorLocation()).Size();
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			Interactive = CurrentInspectiveAgentsPrivate.Get();
		}
	}
	return Interactive;
}

UVSInteractiveFeatureAgent* UVSInteractFeature::GetCurrentInteractiveFeatureAgent() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->CurrentInteractiveAgentPrivate.Get() : nullptr;
}

FName UVSInteractFeature::GetCurrentInteractiveActionFeatureName() const
{
	return InteractFeatureAgentPrivate.IsValid() ? InteractFeatureAgentPrivate->CurrentInteractiveActionFeatureName : NAME_None;
}

UVSInteractiveActionFeature* UVSInteractFeature::GetCurrentInteractiveActionFeature() const
{
	if (!IsInteracting()) return nullptr;
	return InteractFeatureAgentPrivate->CurrentInteractiveAgentPrivate->GetActionFeatureByName(InteractFeatureAgentPrivate->CurrentInteractiveActionFeatureName);
}

bool UVSInteractFeature::IsInspectingOnTarget(UVSInteractiveFeatureAgent* TargetAgent) const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return false;
	if (!InteractFeatureAgentPrivate->CurrentInspectiveAgentsPrivate.Contains(TargetAgent)) return false;
	return true;
}

bool UVSInteractFeature::IsInteracting() const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return false;
	if (!InteractFeatureAgentPrivate->CurrentInteractiveAgentPrivate.IsValid()) return false;
	return InteractFeatureAgentPrivate->CurrentInteractiveAgentPrivate->IsInteractedBySource(GetInteractFeatureAgent());
}

float UVSInteractFeature::GetInteractionTime() const
{
	if (!InteractFeatureAgentPrivate.IsValid()) return false;
	if (!InteractFeatureAgentPrivate->CurrentInteractiveAgentPrivate.IsValid()) return false;
	return InteractFeatureAgentPrivate->CurrentInteractiveAgentPrivate->GetInteractionTime(GetInteractFeatureAgent());
}

void UVSInteractFeature::OnMovementTagsUpdated_Implementation()
{
	
}

void UVSInteractFeature::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	
}
