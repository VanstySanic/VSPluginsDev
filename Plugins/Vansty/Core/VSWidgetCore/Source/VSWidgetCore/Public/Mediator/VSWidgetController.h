// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetChild.h"
#include "Classes/VSTickableObject.h"
#include "VSWidgetController.generated.h"

class UVSWidgetBinder;
class UWidget;

/**
 * Controller object that coordinates widget binding, lifecycle management,
 * and per-frame updates for a group of widget binders.
 *
 * This class acts as the central hub between UI widgets and their associated
 * binders, handling registration, initialization, dynamic rebinding by type,
 * and orderly teardown. It supports instanced binder composition and optional
 * tick-driven updates without requiring a direct widget hierarchy.
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, DisplayName = "VS.Widget.Controller.Base")
class VSWIDGETCORE_API
UVSWidgetController : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

	/** Initializes binders, and rebinds all widgets. */
	UFUNCTION(BlueprintCallable, Category = "Widget Controller", meta = (AutoCreateRefTerm = "TypedWidgetsToBind"))
	void Register();

	/** Unbinds all widgets, and shuts down binders. */
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void Unregister();
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void Reregister();
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	bool IsRegistered() { return bIsRegistered; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	bool HasBeenInitialized() { return bHasBeenInitialized; }

	/** Binds a widget to the given logical type and propagates it to all binders. */
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void BindWidget(UWidget* Widget, const FName TypeName);

	/** Unbinds a previously bound widget of the given type. */
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void UnbindWidget(UWidget* Widget, const FName TypeName);

	/** Rebinds the widget currently associated with the given type name. */
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void RebindWidgetByType(const FName TypeName);

	/** Returns the widget currently bound to the specified type name, if any. */
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	UWidget* GetBoundTypedWidget(const FName TypeName);

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	TMap<FName, UWidget*> GetAllBoundTypedWidgets() const;

protected:
	//~ Begin UVSTickableObject Interface
	virtual void TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction) override;
	//~ End UVSTickableObject Interface

	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void BindTypedWidget(const FName TypeName, UWidget* Widget);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void UnbindTypedWidget(const FName TypeName, UWidget* Widget);


protected:
	/** Binder objects responsible for reacting to bound widget changes. */
	UPROPERTY(EditAnywhere, Instanced, Category = "Widget Controller")
	TArray<TObjectPtr<UVSWidgetBinder>> WidgetBinders;

private:
	uint8 bIsRegistered : 1;
	uint8 bHasBeenInitialized : 1;

	TMap<FName, TWeakObjectPtr<UWidget>> BoundTypedWidgets;
};
