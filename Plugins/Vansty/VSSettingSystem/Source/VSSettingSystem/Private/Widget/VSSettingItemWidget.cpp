// Copyright VanstySanic. All Rights Reserved.

#include "Widget/VSSettingItemWidget.h"
#include "CommonInputSubsystem.h"
#include "CommonRotator.h"
#include "VSSettingSystemUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Items/VSSettingItemBase.h"
#include "Slate/SObjectWidget.h"
#include "Types/VSGameplayTypes.h"
#include "Widgets/SViewport.h"

UVSSettingItemWidget::UVSSettingItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UVSSettingItemWidget::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITOR
	CachedSettingItemSpecifyTag = SettingItemSpecifyTag;
#endif
}

#if WITH_EDITOR
void UVSSettingItemWidget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemWidget, SettingItemSpecifyTag))
	{
		Execute_UnbindWidgetFromSettingItem(this, CachedSettingItemSpecifyTag);
		CachedSettingItemSpecifyTag = SettingItemSpecifyTag;
		Execute_BindWidgetToSettingItem(this, SettingItemSpecifyTag);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemWidget::NativePreConstruct()
{
	GenerateCoreWidget();
	Execute_BindWidgetToSettingItem(this, SettingItemSpecifyTag);
	
	Super::NativePreConstruct();
}

FReply UVSSettingItemWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	CoreWidget->SetFocus();
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UVSSettingItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

FReply UVSSettingItemWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UVSSettingItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
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

void UVSSettingItemWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
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

void UVSSettingItemWidget::NativeOnHighlighted()
{
}

void UVSSettingItemWidget::NativeOnUnhighlighted()
{
}

void UVSSettingItemWidget::HighlightInternal()
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

void UVSSettingItemWidget::UnhighlightInternal()
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

void UVSSettingItemWidget::BindWidgetToSettingItem_Implementation(const FGameplayTag& SpecifyTag)
{
	if (UVSSettingItemBase* SettingItem = UVSSettingSystemUtils::GetSettingItemBySpecifyTag(SpecifyTag))
	{
		SettingItem->BindWidget(TextBlock_Name, FName("Name"));
		
		if (UCommonRotator* Rotator = Cast<UCommonRotator>(CoreWidget))
		{
			SettingItem->BindWidget(Rotator, FName("Value"));
		}
		else if (UComboBoxString* ComboBox = Cast<UComboBoxString>(CoreWidget))
		{
			SettingItem->BindWidget(ComboBox, FName("Value"));
		}
		
		if (CoreWidget.GetClass()->ImplementsInterface(UVSSettingWidgetInterface::StaticClass()))
		{
			Execute_BindWidgetToSettingItem(CoreWidget, SettingItemSpecifyTag);
		}
	}
}

void UVSSettingItemWidget::UnbindWidgetFromSettingItem_Implementation(const FGameplayTag& SpecifyTag)
{
	if (UVSSettingItemBase* SettingItem = UVSSettingSystemUtils::GetSettingItemBySpecifyTag(SpecifyTag))
	{
		SettingItem->UnbindWidget(TextBlock_Name, FName("Name"));
		if (CoreWidget.GetClass()->ImplementsInterface(UVSSettingWidgetInterface::StaticClass()))
		{
			Execute_UnbindWidgetFromSettingItem(CoreWidget, SettingItemSpecifyTag);
		}
		
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(CoreWidget))
		{
			SettingItem->UnbindWidget(ComboBox, FName("Value"));
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(CoreWidget))
		{
			SettingItem->UnbindWidget(Rotator, FName("Value"));
		}
	}
}


void UVSSettingItemWidget::GenerateCoreWidget()
{
	if (CoreWidget)
	{
		CoreWidget->RemoveFromParent();
	}
	if (CoreWidgetClass)
	{
		CoreWidget = WidgetTree->ConstructWidget<UWidget>(CoreWidgetClass);
		SetDesiredFocusWidget(CoreWidget);
		Panel_CoreWidget->AddChild(CoreWidget);
	}
}
