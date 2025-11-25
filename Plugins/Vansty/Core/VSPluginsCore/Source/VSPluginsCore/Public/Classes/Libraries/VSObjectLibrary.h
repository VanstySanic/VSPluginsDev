// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSObjectLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSObjectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object"))
	static bool IsObjectTemplate(UObject* Object);
};
