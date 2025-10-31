// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSTickFunctionOwnerInterface.generated.h"

UINTERFACE(NotBlueprintable, NotBlueprintType)
class UVSTickFunctionOwnerInterface : public UInterface
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
	virtual FTickFunction* GetTickFunctionPtr() const = 0;
};
