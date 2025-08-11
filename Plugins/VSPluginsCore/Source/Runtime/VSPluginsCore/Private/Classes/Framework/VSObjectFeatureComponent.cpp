// Copyright VanstySanic. All Rights Reserved.


#include "Classes/Framework/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSObjectFeature.h"
#include "Libraries/VSGameplayLibrary.h"

UVSObjectFeatureComponent::UVSObjectFeatureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicateUsingRegisteredSubObjectList = true;

	RootFeature = CreateDefaultSubobject<UVSObjectFeature>(TEXT("RootFeature"));
}

void UVSObjectFeatureComponent::OnRegister()
{
	Super::OnRegister();
	
	if (!UVSGameplayLibrary::IsInGame())
	{
		if (RootFeature) { RootFeature->RegisterFeature(); }
	}
}

void UVSObjectFeatureComponent::OnUnregister()
{
	if (!UVSGameplayLibrary::IsInGame())
	{
		if (RootFeature) { RootFeature->UnregisterFeature(); }
	}
	
	Super::OnUnregister();
}

void UVSObjectFeatureComponent::BeginPlay()
{
	Super::BeginPlay();
	if (RootFeature) { RootFeature->RegisterFeature(); }
}

void UVSObjectFeatureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RootFeature) { RootFeature->UnregisterFeature(); }
	Super::EndPlay(EndPlayReason);
}

void UVSObjectFeatureComponent::DestroyComponent(bool bPromoteChildren)
{
	if (RootFeature) { RootFeature->DestroyFeature(); }
	Super::DestroyComponent(bPromoteChildren);
}

UVSObjectFeature* UVSObjectFeatureComponent::GetRootFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const
{
	if (!RootFeature || !RootFeature.IsA(Class)) return nullptr;
	return RootFeature;
}

UVSObjectFeature* UVSObjectFeatureComponent::AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName)
{
	return RootFeature ? RootFeature->AddSubFeatureByClass(Class, OptionalFeatureName) : nullptr;
}

void UVSObjectFeatureComponent::RemoveSubFeature(UVSObjectFeature* InFeature, bool bRecursive)
{
	if (RootFeature) { RootFeature->RemoveSubFeature(InFeature, bRecursive); }
}

TArray<UVSObjectFeature*> UVSObjectFeatureComponent::GetSubFeatures(bool bRecursive) const
{
	return RootFeature ? RootFeature->GetSubFeatures(bRecursive) : TArray<UVSObjectFeature*>();
}

UVSObjectFeature* UVSObjectFeatureComponent::GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive) const
{
	return RootFeature ? RootFeature->GetSubFeatureByClass(Class, bRecursive) : nullptr;
}

TArray<UVSObjectFeature*> UVSObjectFeatureComponent::GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive) const
{
	return RootFeature ? RootFeature->GetSubFeaturesByClass(Class, bRecursive) : TArray<UVSObjectFeature*>();
}

UVSObjectFeature* UVSObjectFeatureComponent::GetSubFeatureByName(FName Name, bool bRecursive) const
{
	return RootFeature ? RootFeature->GetSubFeatureByName(Name, bRecursive) : nullptr;
}
