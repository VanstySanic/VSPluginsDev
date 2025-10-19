// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSHighlightableUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSHighlightableUserWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighlightSignature, UVSHighlightableUserWidget*, Widget);

public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	
	virtual void NativeOnHighlighted();
	virtual void NativeOnUnhighlighted();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool IsHighlighted() const { return bIsHighlighted; }

protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnHighlighted", Category = "Widget")
	void OnWidgetHighlighted();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnUnhighlighted", Category = "Widget")
	void OnWidgetUnhighlighted();

private:
	void HighlightInternal();
	void UnhighlightInternal();

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightSignature OnHighlighted;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightSignature OnUnhighlighted;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bBindFucusToHighlight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bProcessMouseInTypeOnly = true;

private:
	bool bIsHighlighted = false;
	bool bMouseInside = false;
	bool bInFocusPath = false;
};
