// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSHighlightableWidget.generated.h"

/**
 * 
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
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnHighlighted", Category = "Highlight")
	// ReSharper disable once CppUEBlueprintImplementableEventNotImplemented
	void OnWidgetHighlighted();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnUnhighlighted", Category = "Highlight")
	// ReSharper disable once CppUEBlueprintImplementableEventNotImplemented
	void OnWidgetUnhighlighted();

private:
	void HighlightInternal();
	void UnhighlightInternal();
	
public:
	FItemWidgetDelegate OnHighlighted_Native;
	FItemWidgetDelegate OnUnhighlighted_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightEvent OnHighlighted;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightEvent OnUnhighlighted;

private:
	uint8 bIsHighlighted : 1;
	uint8 bMouseHover : 1;
	uint8 bInFocusPath : 1;
};
