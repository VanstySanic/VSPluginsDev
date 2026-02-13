// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSGameplayTagFeatureInterface.generated.h"

struct FGameplayTagContainer;
class UVSGameplayTagFeatureBase;

UINTERFACE()
class VSPLUGINSCORE_API UVSGameplayTagFeatureInterface : public UInterface
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
	UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Native() const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName = "Get Primary Gameplay Tag Feature", Category = "GameplayTags")
	UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature() const;

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagFeatureTagsUpdated(UVSGameplayTagFeatureBase* GameplayTagFeature);

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagFeatureTagEventsNotified(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents);

protected:
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const;
};
