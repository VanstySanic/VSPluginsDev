// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSCharacterMovementSystemUtils.generated.h"

/**
 * 
 */
UCLASS()
class VSCHARACTERMOVEMENTSYSTEM_API UVSCharacterMovementSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Movement")
	static FGameplayTag GetTaggedMovementMode(const EMovementMode InMovementMode, const uint8 InCustomMode = 0);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	static bool GetUntaggedMovementMode(const FGameplayTag& InTag, TEnumAsByte<EMovementMode>& OutMovementMode, uint8& OutCustomMode);
};
