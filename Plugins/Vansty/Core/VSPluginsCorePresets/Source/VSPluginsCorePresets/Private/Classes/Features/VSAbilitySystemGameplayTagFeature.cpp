// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSAbilitySystemGameplayTagFeature.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Classes/Settings/VSPluginsCoreGameSettings.h"
#include "VSPluginsCoreCpp/Public/VSPrivablic.h"

using GameplayTagTagCountMap = TMap<FGameplayTag, int32>;

VS_DECLARE_PRIVABLIC_MEMBER(UAbilitySystemComponent, GameplayTagCountContainer, FGameplayTagCountContainer);
VS_DECLARE_PRIVABLIC_MEMBER(FGameplayTagCountContainer, ExplicitTags, FGameplayTagContainer);
VS_DECLARE_PRIVABLIC_MEMBER(FGameplayTagCountContainer, ExplicitTagCountMap, GameplayTagTagCountMap);

UVSAbilitySystemGameplayTagFeature::UVSAbilitySystemGameplayTagFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSAbilitySystemGameplayTagFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	AbilitySystemComponentPrivate = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwnerActor());
	check(AbilitySystemComponentPrivate.IsValid());
	AbilitySystemGameplayTagCountContainerPtr = &VS_PRIVABLIC_MEMBER(AbilitySystemComponentPrivate.Get(), UAbilitySystemComponent, GameplayTagCountContainer);
	check(AbilitySystemGameplayTagCountContainerPtr);
	
	/** Bind listen delegate. */
	for (const FGameplayTag& GameplayTag : UVSPluginsCoreGameSettings::Get()->AbilitySystemListeningTags.GetGameplayTagArray())
	{
		FDelegateHandle DelegateHandle = AbilitySystemComponentPrivate->RegisterGameplayTagEvent(GameplayTag, EGameplayTagEventType::AnyCountChange)
			.AddUObject(this, &UVSAbilitySystemGameplayTagFeature::OnAbilitySystemListeningTagsUpdated);
		AbilitySystemRegisteredDelegateHandles.Emplace(GameplayTag, DelegateHandle);
	}
}

void UVSAbilitySystemGameplayTagFeature::Uninitialize_Implementation()
{
	if (AbilitySystemComponentPrivate.IsValid())
	{
		for (const TPair<FGameplayTag, FDelegateHandle>& AbilitySystemComponentRegisteredDelegateHandle : AbilitySystemRegisteredDelegateHandles)
		{
			if (AbilitySystemComponentRegisteredDelegateHandle.Value.IsValid())
			{
				AbilitySystemComponentPrivate->UnregisterGameplayTagEvent(AbilitySystemComponentRegisteredDelegateHandle.Value, AbilitySystemComponentRegisteredDelegateHandle.Key, EGameplayTagEventType::AnyCountChange);
			}
		}
	}
	
	AbilitySystemGameplayTagCountContainerPtr = nullptr;
	AbilitySystemComponentPrivate.Reset();
	AbilitySystemRegisteredDelegateHandles.Empty();
	
	Super::Uninitialize_Implementation();
}

FGameplayTagContainer& UVSAbilitySystemGameplayTagFeature::GetGameplayTagContainerSourceReference()
{
	check(AbilitySystemGameplayTagCountContainerPtr);
	return VS_PRIVABLIC_MEMBER(AbilitySystemGameplayTagCountContainerPtr, FGameplayTagCountContainer, ExplicitTags);
}

TMap<FGameplayTag, int32>& UVSAbilitySystemGameplayTagFeature::GetGameplayTagCountMapSourceReference()
{
	check(AbilitySystemGameplayTagCountContainerPtr);
	return VS_PRIVABLIC_MEMBER(AbilitySystemGameplayTagCountContainerPtr, FGameplayTagCountContainer, ExplicitTagCountMap);
}

void UVSAbilitySystemGameplayTagFeature::ModifyTagCount(const FGameplayTag& GameplayTag, int32 NewCount)
{
	if (AbilitySystemComponentPrivate.IsValid())
	{
		AbilitySystemComponentPrivate->SetTagMapCount(GameplayTag, NewCount);
	}
}

void UVSAbilitySystemGameplayTagFeature::OnAbilitySystemListeningTagsUpdated(const FGameplayTag Tag, int32 Count)
{
	MarkTagsDirty();
	if (bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}
