// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSHighlightableWidget.generated.h"

/**
 * User widget that exposes a unified highlighted/unhighlighted state.
 *
 * Highlight state is driven by mouse hover (mouse mode) and focus-path
 * changes (keyboard/gamepad mode), then forwarded to native and Blueprint
 * delegates/events.
 */
UCLASS()
class VSWIDGETCORE_API UVSHighlightableWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FItemWidgetDelegate, UVSHighlightableWidget* /** Widget */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightEvent, UVSHighlightableWidget*, Widget);

public:
	//~ Begin UUserWidget Interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	//~ End UUserWidget Interface

	virtual void NativeOnHighlighted();
	virtual void NativeOnUnhighlighted();
	
public:
	/** Called when this widget enters highlighted state. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnHighlighted", Category = "Highlight")
	// ReSharper disable once CppUEBlueprintImplementableEventNotImplemented
	void OnWidgetHighlighted();

	/** Called when this widget leaves highlighted state. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnUnhighlighted", Category = "Highlight")
	// ReSharper disable once CppUEBlueprintImplementableEventNotImplemented
	void OnWidgetUnhighlighted();

private:
	void HighlightInternal();
	void UnhighlightInternal();
	
public:
	/** Broadcast when highlighted. */
	FItemWidgetDelegate OnHighlighted_Native;
	/** Broadcast when unhighlighted. */
	FItemWidgetDelegate OnUnhighlighted_Native;
	
	/** Broadcast when highlighted. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightEvent OnHighlighted;

	/** Broadcast when unhighlighted. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightEvent OnUnhighlighted;

private:
	uint8 bIsHighlighted : 1;
	uint8 bMouseHover : 1;
	uint8 bInFocusPath : 1;
};
