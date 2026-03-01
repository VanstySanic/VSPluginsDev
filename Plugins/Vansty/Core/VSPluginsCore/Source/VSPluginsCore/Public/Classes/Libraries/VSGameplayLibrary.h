// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSGameplayLibrary.generated.h"

/**
 * Blueprint gameplay utility helpers.
 */
UCLASS()
class VSPLUGINSCORE_API UVSGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** 
	 * Tries to find the currently active primary Game or Play in Editor world.
	 * @remarks This should only be called if you do not have a reliable world context object to use, and do not use it for gameplay objects.
	 * @remarks This does not work well if you open multiple clients in one-process editor.
	 * @return Either nullptr or a World that is guaranteed to be of type Game or PIE
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static UWorld* GetPossibleGameplayWorld();
};
