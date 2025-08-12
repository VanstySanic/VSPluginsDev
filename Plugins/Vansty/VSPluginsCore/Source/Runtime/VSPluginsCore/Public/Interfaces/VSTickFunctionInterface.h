// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSTickFunctionInterface.generated.h"

UINTERFACE(NotBlueprintable, NotBlueprintType)
class UVSTickFunctionInterface : public UInterface
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
	virtual FTickFunction* GetTickFunctionPtr() const = 0;
};
