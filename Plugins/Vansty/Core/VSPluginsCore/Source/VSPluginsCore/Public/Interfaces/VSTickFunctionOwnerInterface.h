// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSTickFunctionOwnerInterface.generated.h"

UINTERFACE(NotBlueprintable)
class VSPLUGINSCORE_API UVSTickFunctionOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSPLUGINSCORE_API IVSTickFunctionOwnerInterface
{
	GENERATED_BODY()

public:
	virtual FTickFunction* GetPrimaryTickFunctionPtr() const = 0;
};
