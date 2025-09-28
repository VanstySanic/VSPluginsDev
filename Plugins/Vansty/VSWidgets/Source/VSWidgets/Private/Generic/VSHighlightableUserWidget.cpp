// Copyright VanstySanic. All Rights Reserved.

#include "Generic/VSHighlightableUserWidget.h"
#include "CommonInputSubsystem.h"
#include "Slate/SObjectWidget.h"
#include "Types/VSGameplayTypes.h"

UVSHighlightableUserWidget::UVSHighlightableUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSHighlightableUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	bMouseInside = true;

	if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
	{
		if ((!bProcessMouseInTypeOnly || InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard))
		{
			HighlightInternal();
		}
	}
}

FReply UVSHighlightableUserWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.GetCursorDelta().IsNearlyZero(0.01f))
	{
		if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
		{
			if ((!bProcessMouseInTypeOnly || InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard))
			{
				HighlightInternal();
			}
		}
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UVSHighlightableUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bMouseInside = false;
	
	// if (!bInFocusPath || !bBindFucusToHighlight)
	if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
	{
		if (!bInFocusPath || (!bProcessMouseInTypeOnly || InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard))
		{
			UnhighlightInternal();
		}
	}
}

void UVSHighlightableUserWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	TSharedPtr<SObjectWidget> SafeGCWidget = MyGCWidget.Pin();
	if (SafeGCWidget.IsValid())
	{
		const bool bDecendantNewlyFocused = NewWidgetPath.ContainsWidget(SafeGCWidget.Get());
		if (bDecendantNewlyFocused && bBindFucusToHighlight)
		{
			bInFocusPath = true;
			HighlightInternal();
		}
		else if (!bDecendantNewlyFocused && bBindFucusToHighlight)
		{
			bInFocusPath = false;
			if (!bMouseInside || bBindFucusToHighlight)
			{
				UnhighlightInternal();
			}
		}
	}
}

void UVSHighlightableUserWidget::NativeOnHighlighted()
{
}

void UVSHighlightableUserWidget::NativeOnUnhighlighted()
{
}

void UVSHighlightableUserWidget::HighlightInternal()
{
	if (bIsHighlighted) return;
	bIsHighlighted = true;
	if (bBindFucusToHighlight && !bInFocusPath)
	{
		SetFocus();
	}
	NativeOnHighlighted();
	OnWidgetHighlighted();
}

void UVSHighlightableUserWidget::UnhighlightInternal()
{
	if (!bIsHighlighted) return;
	bIsHighlighted = false;
	if (bBindFucusToHighlight && bInFocusPath)
	{
		FSlateApplication::Get().ClearUserFocus(FVSUserQueryParams(GetOwningPlayer()).GetUserIndex());
	}
	NativeOnUnhighlighted();
	OnWidgetUnhighlighted();
}
