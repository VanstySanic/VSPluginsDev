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

public:
	virtual UVSGameplayTagController* GetGameplayTagController() const { return nullptr; }
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName = "GetGameplayTagController", Category = "GameplayTags")
	UVSGameplayTagController* GetGameplayTagController_BP() const;

private:
	virtual UVSGameplayTagController* GetGameplayTagController_BP_Implementation() const final;
};
