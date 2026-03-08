// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSMoverSystemUtils.generated.h"

/**
 * 
 */
UCLASS()
class VSMOVERSYSTEM_API UVSMoverSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Movement")
	static FGameplayTag GetUnnamedMovementMode(const FName ModeName);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	static FName GetNamedMovementMode(const FGameplayTag& InTag);
};
