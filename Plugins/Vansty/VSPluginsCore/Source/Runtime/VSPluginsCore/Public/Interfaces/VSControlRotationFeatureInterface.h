// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSControlRotationFeatureInterface.generated.h"

class UVSControlRotationFeature;

UINTERFACE()
class VSPLUGINSCORE_API UVSControlRotationFeatureInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSPLUGINSCORE_API IVSControlRotationFeatureInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Features")
	UVSControlRotationFeature* GetControlRotationFeature() const;
};
