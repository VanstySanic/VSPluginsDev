// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Components/VSObjectFeatureComponent.h"
#include "Classes/VSObjectFeature.h"
#include "Net/UnrealNetwork.h"

UVSFeatureComponent::UVSFeatureComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bRegisterFeaturesDuringBeginPlay(true)
{
	bReplicateUsingRegisteredSubObjectList = true;
}

void UVSFeatureComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != nullptr) { BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps); }

	DOREPLIFETIME(UVSFeatureComponent, Features);
}

void UVSFeatureComponent::BeginDestroy()
{
	for (UVSObjectFeature* Feature : Features)
	{
		if (Feature)
		{
			Feature->DestroyFeature();
		}
	}

	Features.Empty();
	
	Super::BeginDestroy();
}

void UVSFeatureComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bRegisterFeaturesDuringBeginPlay)
	{
		RegisterFeatures();
	}
}

void UVSFeatureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (UVSObjectFeature* Feature : Features)
	{
		if (Feature && Feature->IsRegistered())
		{
			Feature->UnregisterFeature();
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UVSFeatureComponent::RegisterFeatures()
{
	TArray<UVSObjectFeature*> CopiedFeatures = Features;
	for (UVSObjectFeature* Feature : CopiedFeatures)
	{
		if (Feature && Feature->bRegisterWithOwner && !Feature->IsRegistered())
		{
			Feature->RegisterFeature();
		}
	}
}

void UVSFeatureComponent::UnregisterFeatures()
{
	TArray<UVSObjectFeature*> CopiedFeatures = Features;
	for (UVSObjectFeature* Feature : CopiedFeatures)
	{
		if (Feature && Feature->IsRegistered())
		{
			Feature->UnregisterFeature();
		}
	}
}

TArray<UVSObjectFeature*> UVSFeatureComponent::GetFeatures() const
{
	TArray<UVSObjectFeature*> CopiedFeatures = Features;
	for (UVSObjectFeature* Feature : CopiedFeatures)
	{
		if (Feature)
		{
			CopiedFeatures.Add(Feature);
			CopiedFeatures.Append(Feature->GetSubFeatures());
		}
	}
	return CopiedFeatures;
}

bool UVSFeatureComponent::HasFeature(UVSObjectFeature* Feature) const
{
	if (Features.Contains(Feature)) return true;
	for (UVSObjectFeature* DirectFeature : Features)
	{
		if (DirectFeature && DirectFeature->HasSubFeature(Feature)) return true;
	}

	return false;
}

void UVSFeatureComponent::AddFeatureInstance(UVSObjectFeature* Feature, bool bDeferRegister)
{
	if (!Feature) return;
	
	if (Feature->GetOwnerActor() != GetOwner())
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("AddInstancedFeature: (%s) has different owner actor from (%s). Aborting."), *GetPathName(), *Feature->GetPathName());
		return;
	}
	
	/** Force re-register sub feature when reparenting. */
	if (Feature->GetOwnerFeature())
	{
		Feature->GetOwnerFeature()->RemoveSubFeature(Feature);
	}
	else if (Feature->IsRegistered())
	{
		Feature->UnregisterFeature();
	}

	Features.Add(Feature);

	const bool bActorBeginPlay = !GetOwner() || GetOwner()->IsActorBeginningPlay() || GetOwner()->HasActorBegunPlay();
	if (!bDeferRegister && Feature->bRegisterWithOwner && bActorBeginPlay)
	{
		Feature->RegisterFeature();
	}
}

UVSObjectFeature* UVSFeatureComponent::AddFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bDeferRegister)
{
	if (!Class) return nullptr;
	UVSObjectFeature* Feature = NewObject<UVSObjectFeature>(this, Class);
	
	Features.Add(Feature);
	
	const bool bActorBeginPlay = !GetOwner() || GetOwner()->IsActorBeginningPlay() || GetOwner()->HasActorBegunPlay();
	if (!bDeferRegister && Feature->bRegisterWithOwner && bActorBeginPlay)
	{
		Feature->RegisterFeature();
	}
	
	return Feature;
}

void UVSFeatureComponent::RemoveFeature(UVSObjectFeature* Feature)
{
	if (!Feature || !HasFeature(Feature)) return;

	if (Feature->GetOwnerFeature())
	{
		Feature->GetOwnerFeature()->RemoveSubFeature(Feature);
	}
	else
	{
		if (Feature->IsRegistered())
		{
			Feature->UnregisterFeature();
		}
		Features.RemoveSingle(Feature);
	}
}

UVSObjectFeature* UVSFeatureComponent::GetFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const
{
	for (UVSObjectFeature* Feature : Features)
	{
		if (Feature && Feature->IsA(Class))
		{
			return Feature;
		}
	}

	for (UVSObjectFeature* Feature : Features)
	{
		if (Feature)
		{
			if (UVSObjectFeature* SubFeature = Feature->GetSubFeatureByClass(Class))
			{
				return SubFeature;
			}
		}
	}
	
	return nullptr;
}

TArray<UVSObjectFeature*> UVSFeatureComponent::GetFeaturesByClass(TSubclassOf<UVSObjectFeature> Class) const
{
	TArray<UVSObjectFeature*> OutFeatures;
	for (UVSObjectFeature* Feature : GetFeatures())
	{
		if (Feature->IsA(Class))
		{
			OutFeatures.Add(Feature);
		}
	}
	return OutFeatures;
}

UVSObjectFeature* UVSFeatureComponent::GetFeatureByName(FName Name) const
{
	for (UVSObjectFeature* Feature : Features)
	{
		if (Feature && Feature->FeatureName == Name)
		{
			return Feature;
		}
	}

	for (UVSObjectFeature* Feature : Features)
	{
		if (Feature)
		{
			if (UVSObjectFeature* SubFeature = Feature->GetSubFeatureByName(Name))
			{
				return SubFeature;
			}
		}
	}
	
	return nullptr;
}
