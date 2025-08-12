// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSInteractSystemUtils.generated.h"

class UVSInteractFeatureAgent;
class UVSInteractiveFeatureAgent;

/**
 * 
 */
UCLASS()
class VSINTERACTSYSTEM_API UVSInteractSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interact")
	static UVSInteractFeatureAgent* GetInteractFeatureAgentFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactive")
	static UVSInteractiveFeatureAgent* GetInteractiveFeatureAgentFromActor(AActor* Actor);
};
