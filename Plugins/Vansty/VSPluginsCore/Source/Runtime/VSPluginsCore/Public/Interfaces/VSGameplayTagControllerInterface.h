// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSGameplayTagControllerInterface.generated.h"

class UVSGameplayTagController;

UINTERFACE(MinimalAPI)
class UVSGameplayTagControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSPLUGINSCORE_API IVSGameplayTagControllerInterface
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayTags")
	UVSGameplayTagController* GetGameplayTagController() const;

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagsUpdated() const;

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayTags")
	void OnGameplayTagEventNotified();
};
