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

protected:
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
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

protected:
	/** The widget switcher that contains the Panel ans WidgetStack. Optional. User can switch between this. */
	UPROPERTY(BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	/** The panel that contains the primary contents. Optional. Needs to be in WidgetSwitcher. */
	UPROPERTY(BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> Panel;

	/** Push activable widgets and process input here. Optional. Needs to be in WidgetSwitcher. */
	UPROPERTY(BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> WidgetStack;

	/** Optional. Need to be outside the widget switcher, on the top layer. */
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UOverlay> Overlay;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bRestoreActiveUIInputConfig = true;

private:
	FUIInputConfig CachedUIInputConfig;
	int32 WidgetNumInStackLastFrame = 0;
};
