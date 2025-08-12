// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSInteractiveInterface.generated.h"

class UVSInteractiveFeatureAgent;

UINTERFACE()
class UVSInteractiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSINTERACTSYSTEM_API IVSInteractiveInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactive")
	UVSInteractiveFeatureAgent* GetInteractiveFeatureAgent() const;
};
