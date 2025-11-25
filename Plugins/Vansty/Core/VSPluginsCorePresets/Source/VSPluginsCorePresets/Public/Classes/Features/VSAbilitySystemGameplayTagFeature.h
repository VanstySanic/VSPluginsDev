// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "VSAbilitySystemGameplayTagFeature.generated.h"

struct FGameplayTagCountContainer;

/**
 * UVSAbilitySystemGameplayTagFeature
 *
 * Gameplay-tag feature backed by an AbilitySystemComponent.
 *
 * - Uses the ASC’s internal FGameplayTagCountContainer as the tag source.
 * - Forwards all read/write operations to the ASC’s explicit tag and count maps.
 * - Mirrors ASC tag changes via registered GameplayTagEvent callbacks.
 * - Integrates ASC tag state into the base feature’s event and replication flow.
 *
 * Responsibilities:
 * - Locate and cache the owning ASC on initialize.
 * - Bind to configured ASC tag events and mark the feature dirty on updates.
 * - Use SetTagMapCount() to apply authoritative modifications.
 * - Provide the base class with references to the ASC’s tag container/count map.
 *
 * Suitable when:
 * - The feature should reflect and manipulate real ASC tag state.
 * - You need the base feature’s network-aware APIs and notifications
 *   while retaining the ASC as the authoritative tag storage.
 */
UCLASS(DisplayName = "VS.Feature.GameplayTags.AbilitySystem")
class VSPLUGINSCOREPRESETS_API UVSAbilitySystemGameplayTagFeature : public UVSGameplayTagFeatureBase
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.

	//~ Begin UVSGameplayTagFeature Interface.
	virtual FGameplayTagContainer& GetGameplayTagContainerSourceReference() override;
	virtual TMap<FGameplayTag, int32>& GetGameplayTagCountMapSourceReference() override;
	virtual void ModifyTagCount(const FGameplayTag& GameplayTag, int32 NewCount) override;
	//~ End UVSGameplayTagFeature Interface.

private:
	UFUNCTION()
	void OnAbilitySystemListeningTagsUpdated(const FGameplayTag Tag, int32 Count);
	
private:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponentPrivate;
	FGameplayTagCountContainer* AbilitySystemGameplayTagCountContainerPtr = nullptr;
	TMap<FGameplayTag, FDelegateHandle> AbilitySystemRegisteredDelegateHandles;
};
