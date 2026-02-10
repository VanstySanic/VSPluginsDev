// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSWidgetBinder.generated.h"

class UWidget;

/**
 * Base binder object responsible for reacting to widget bindings managed
 * by a widget controller.
 *
 * A widget binder encapsulates a specific piece of binding logic, receiving
 * notifications when widgets of certain logical types are bound or unbound,
 * and optionally updating its state every tick via the owning controller.
 *
 * This class is intended to be subclassed for concrete binding behavior,
 * while lifecycle and widget ownership are coordinated externally.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "VS.Widget.Binder.Base")
class VSWIDGETCORE_API UVSWidgetBinder : public UObject
{
	GENERATED_UCLASS_BODY()
	
	friend class UVSWidgetController;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void BindTypedWidget(const FName TypeName, UWidget* Widget);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void UnbindTypedWidget(const FName TypeName, UWidget* Widget);

	/** Works as tick. Called in parent widget controller. */
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Binder")
	void UpdateBinder(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	void RebindWidgetByType(FName TypeName);

	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	void RebindAllWidgets();

	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	UWidget* GetBoundTypedWidget(const FName TypeName) const;
		
	UFUNCTION(BlueprintCallable, Category = "Widget Binder")
	UVSWidgetController* GetWidgetController() const { return WidgetControllerPrivate.Get(); }
	
private:
	TWeakObjectPtr<UVSWidgetController> WidgetControllerPrivate;
};
