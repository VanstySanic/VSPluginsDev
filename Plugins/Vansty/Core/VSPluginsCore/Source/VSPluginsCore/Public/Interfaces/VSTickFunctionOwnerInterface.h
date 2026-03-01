// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSTickFunctionOwnerInterface.generated.h"

/**
 * Interface marker for types that expose a primary tick function pointer.
 */
UINTERFACE(NotBlueprintable)
class VSPLUGINSCORE_API UVSTickFunctionOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Native-only interface used by custom tick utilities to access the owner tick function.
 */
class VSPLUGINSCORE_API IVSTickFunctionOwnerInterface
{
	GENERATED_BODY()

public:
	virtual FTickFunction* GetPrimaryTickFunctionPtr() const = 0;
};
