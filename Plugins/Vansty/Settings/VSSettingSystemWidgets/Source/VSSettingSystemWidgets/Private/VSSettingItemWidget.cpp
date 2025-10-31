// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingItemWidget.h"
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
	Super::NativePreConstruct();

	SetCoreWidgetByClass(CoreWidgetClass);
	Execute_BindWidgetToSettingItem(this, SettingItemSpecifyTag);
}

void UVSSettingItemWidget::NativeDestruct()
{
	Execute_UnbindWidgetFromSettingItem(this, SettingItemSpecifyTag);
	
	Super::NativeDestruct();
}

FReply UVSSettingItemWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	CoreWidget->SetFocus();
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

UWidget* UVSSettingItemWidget::SetCoreWidgetByClass(TSubclassOf<UWidget> Class)
{
	bool bPrevFocused = false;
	if (CoreWidget)
	{
		if (CoreWidget.GetClass()->ImplementsInterface(UVSSettingWidgetInterface::StaticClass()))
		{
			Execute_UnbindWidgetFromSettingItem(CoreWidget, SettingItemSpecifyTag);
		}
		
		CoreWidget->RemoveFromParent();
		bPrevFocused = CoreWidget->HasAnyUserFocus() || CoreWidget->HasFocusedDescendants();
	}
	if (Class)
	{
		CoreWidget = WidgetTree->ConstructWidget<UWidget>(Class);
		Panel_CoreWidget->AddChild(CoreWidget);

		if (Class->ImplementsInterface(UVSSettingWidgetInterface::StaticClass()))
		{
			Execute_BindWidgetToSettingItem(CoreWidget, SettingItemSpecifyTag);
		}
		
		SetDesiredFocusWidget(CoreWidget);
		if (bPrevFocused)
		{
			CoreWidget->SetFocus();
		}
	}

	return CoreWidget;
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
