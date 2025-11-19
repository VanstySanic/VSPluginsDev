// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSGameplayTagFeatureInterface.generated.h"

struct FGameplayTagContainer;
class UVSGameplayTagFeature;

UINTERFACE()
class UVSGameplayTagFeatureInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSPLUGINSCORE_API IVSGameplayTagFeatureInterface
{
	GENERATED_BODY()

public:
	UVSGameplayTagFeature* GetPrimaryGameplayTagFeature_Native() const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName = "Get Primary Gameplay Tag Feature", Category = "GameplayTags")
	UVSGameplayTagFeature* GetPrimaryGameplayTagFeature() const;

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagFeatureTagsUpdated(UVSGameplayTagFeature* GameplayTagFeature);

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagFeatureTagEventsNotified(UVSGameplayTagFeature* GameplayTagFeature, const FGameplayTagContainer& TagEvents);
};
