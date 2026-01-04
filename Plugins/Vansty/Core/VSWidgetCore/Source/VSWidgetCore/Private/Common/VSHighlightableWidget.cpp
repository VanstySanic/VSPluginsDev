// Copyright VanstySanic. All Rights Reserved.

#include "Common/VSHighlightableWidget.h"
#include "CommonInputSubsystem.h"
#include "Slate/SObjectWidget.h"

UVSHighlightableWidget::UVSHighlightableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSHighlightableWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	bMouseHover = true;

	if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
	{
		if (InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard)
		{
			HighlightInternal();
		}
	}
}

FReply UVSHighlightableWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.GetCursorDelta().IsNearlyZero(0.01f))
	{
		if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
		{
			if (InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard)
			{
				HighlightInternal();
			}
		}
	}
	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UVSHighlightableWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bMouseHover = false;

	if (UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
	{
		if (InputSubsystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard)
		{
			UnhighlightInternal();
		}
	}
}

void UVSHighlightableWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	TSharedPtr<SObjectWidget> SafeGCWidget = MyGCWidget.Pin();
	if (SafeGCWidget.IsValid())
	{
		bInFocusPath = NewWidgetPath.ContainsWidget(SafeGCWidget.Get());
		if (bInFocusPath)
		{
			HighlightInternal();
		}
		else
		{
			if (!bMouseHover)
			{
				UnhighlightInternal();
			}
		}
	}
}

void UVSHighlightableWidget::NativeOnHighlighted()
{
}

void UVSHighlightableWidget::NativeOnUnhighlighted()
{
}

void UVSHighlightableWidget::HighlightInternal()
{
	if (bIsHighlighted) return;
	bIsHighlighted = true;
	
	NativeOnHighlighted();
	OnWidgetHighlighted();

	OnHighlighted.Broadcast(this);
}

void UVSHighlightableWidget::UnhighlightInternal()
{
	if (!bIsHighlighted) return;
	bIsHighlighted = false;

	NativeOnUnhighlighted();
	OnWidgetUnhighlighted();

	OnUnhighlighted.Broadcast(this);
}
