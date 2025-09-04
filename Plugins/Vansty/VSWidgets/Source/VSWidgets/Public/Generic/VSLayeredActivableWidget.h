// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "VSLayeredActivableWidget.generated.h"

class UOverlay;
class UCommonActivatableWidgetStack;
class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSLayeredActivableWidget : public UCommonActivatableWidget
{
	GENERATED_UCLASS_BODY()

public:
	virtual void NativeDestruct() override;
	
	/** Get the widget switcher that contains the Panel ans WidgetStack. User can switch between this. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UWidgetSwitcher* GetWidgetSwitcher() const { return WidgetSwitcher; }

	UFUNCTION(BlueprintCallable, Category = "Widget")
	UOverlay* GetOverlayWidget() const { return Overlay; }
	
	/** Get the panel that contains the primary contents. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UPanelWidget* GetPanelWidget() const { return Panel; }

	/** Get the widget stack that user can push activable widgets to and process input in. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UCommonActivatableWidgetStack* GetWidgetStack() const { return WidgetStack; }

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SwitchToPanel();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SwitchToWidgetStack();
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

protected:
	/** The widget switcher that contains the Panel ans WidgetStack. Optional. User can switch between this. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	/** Optional. Need to be outside the widget switcher, on the top layer. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UOverlay> Overlay;

	/** The panel that contains the primary contents. Optional. Needs to be in WidgetSwitcher. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> Panel;

	/** Push activable widgets and process input here. Optional. Needs to be in WidgetSwitcher. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> WidgetStack;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bRestoreActiveUIInputConfig = true;

private:
	FUIInputConfig CachedUIInputConfig;
};
