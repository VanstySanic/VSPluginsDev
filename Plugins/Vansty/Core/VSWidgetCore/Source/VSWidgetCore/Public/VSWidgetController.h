// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetChild.h"
#include "Classes/VSTickableObject.h"
#include "VSWidgetController.generated.h"

class UWidget;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class VSWIDGETCORE_API UVSWidgetController : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()


public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "TypedWidgetsToBind"))
	void Register(const TMap<FName, UWidget*>& TypedWidgetsToBind);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Unregister();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Reregister();
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool IsRegistered() { return bIsRegistered; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool HasBeenInitialized() { return bHasBeenInitialized; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void BindWidget(UWidget* Widget, const FName TypeName);
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void UnbindWidget(UWidget* Widget, const FName TypeName);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	UWidget* GetTypedBoundWidget(const FName TypeName);
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Widget")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Widget")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget")
	void BindTypedWidget(const FName TypeName, UWidget* Widget);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget")
	void UnbindTypedWidget(const FName TypeName, UWidget* Widget);
	

private:
	uint8 bIsRegistered : 1;
	uint8 bHasBeenInitialized : 1;

	TMap<FName, TWeakObjectPtr<UWidget>> TypedBoundWidgets;
};
