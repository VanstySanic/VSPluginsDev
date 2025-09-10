// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSWidgetLibrary.generated.h"

class UWidget;
class UCommonButtonBase;
class UCommonRotator;
struct FVSCommonButtonDisplayParams;
struct FUIInputConfig;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Widget")
	static int32 GetViewportMaxWidgetZOrder(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetWidgetDesiredFocusedWidget(UUserWidget* Source, UWidget* Desired);
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	static FUIInputConfig GetCurrentUIInputConfig(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "NewConfig"))
	static void SetCurrentUIInputConfig(APlayerController* PlayerController, const FUIInputConfig& NewConfig);
	
	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "Params"))
	static void ApplyCommonButtonDisplayParams(UCommonButtonBase* Button, const FVSCommonButtonDisplayParams& Params);

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "Params"))
	static void SetCommonRotatorSelectedItemWithNotification(UCommonRotator* Rotator, int32 NewIndex);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Widget")
	static TArray<FText> GetCommonRotatorOptions(UCommonRotator* Rotator);
};
