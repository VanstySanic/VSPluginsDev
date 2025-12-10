// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSTickFunctionInterface.generated.h"

UINTERFACE(NotBlueprintable)
class VSPLUGINSCORE_API UVSTickFunctionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSPLUGINSCORE_API IVSTickFunctionInterface
{
	GENERATED_BODY()

public:
	virtual FTickFunction* GetPrimaryTickFunctionPtr() const = 0;
};
