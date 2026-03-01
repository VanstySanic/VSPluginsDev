// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSGameplayTagFeatureInterface.generated.h"

struct FGameplayTagContainer;
class UVSGameplayTagFeatureBase;

/**
 * UInterface marker for objects that consume gameplay tag feature callbacks.
 */
UINTERFACE()
class VSPLUGINSCORE_API UVSGameplayTagFeatureInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Contract for resolving and receiving updates from a primary gameplay tag feature.
 */
class VSPLUGINSCORE_API IVSGameplayTagFeatureInterface
{
	GENERATED_BODY()

public:
	/** Native helper that resolves the primary gameplay-tag feature for this object. */
	UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Native() const;
	
protected:
	/** Blueprint-overridable resolver for the primary gameplay-tag feature. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayTags")
	UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature() const;

	/** Callback when the feature flushes tag-state updates. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagFeatureTagsUpdated(UVSGameplayTagFeatureBase* GameplayTagFeature);

	/** Callback when the feature emits gameplay-tag event notifications. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagFeatureTagEventsNotified(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents);

protected:
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const;
};
