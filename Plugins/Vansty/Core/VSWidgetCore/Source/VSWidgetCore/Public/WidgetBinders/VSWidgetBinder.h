// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSWidgetBinder.generated.h"

class UWidget;

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class VSWIDGETCORE_API UVSWidgetBinder : public UObject
{
	GENERATED_UCLASS_BODY()
	
	friend class UVSWidgetController;

public:
	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	UVSWidgetController* GetWidgetController() const { return WidgetControllerPrivate.Get(); }
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void BindTypedWidget(const FName TypeName, UWidget* Widget);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void UnbindTypedWidget(const FName TypeName, UWidget* Widget);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void UpdateBinder(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	void RebindWidgetByType(FName TypeName);

	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	void RebindAllWidgets();

private:
	TWeakObjectPtr<UVSWidgetController> WidgetControllerPrivate;
};
