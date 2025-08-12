// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSInteractInterface.generated.h"

class UVSInteractFeatureAgent;

UINTERFACE()
class UVSInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSINTERACTSYSTEM_API IVSInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	UVSInteractFeatureAgent* GetInteractFeatureAgent() const;
};
