// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSOnlineSystemUtils.generated.h"

/**
 * 
 */
UCLASS()
class VSONLINESYSTEM_API UVSOnlineSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	static FName GetCurrentSessionName();
};
