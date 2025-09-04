// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSWidgetUtils.generated.h"

struct FUIInputConfig;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSWidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	static FUIInputConfig GetCurrentUIInputConfig(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "NewConfig"))
	static void SetCurrentUIInputConfig(APlayerController* PlayerController, const FUIInputConfig& NewConfig);
};
