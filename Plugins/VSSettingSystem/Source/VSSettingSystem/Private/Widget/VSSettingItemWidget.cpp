// Copyright VanstySanic. All Rights Reserved.

#include "Widget/VSSettingItemWidget.h"
#include "VSSettingSystemUtils.h"
#include "Components/ComboBoxString.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Items/VSSettingItemBase.h"

UVSSettingItemWidget::UVSSettingItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
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
	
	if (IsDesignTime())
	{
		GenerateCoreWidget();
		Execute_BindWidgetToSettingItem(this, SettingItemSpecifyTag);
	}
}

void UVSSettingItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GenerateCoreWidget();
	Execute_BindWidgetToSettingItem(this, SettingItemSpecifyTag);
}

void UVSSettingItemWidget::BindWidgetToSettingItem_Implementation(const FGameplayTag& SpecifyTag)
{
	if (UVSSettingItemBase* SettingItem = UVSSettingSystemUtils::GetSettingItemBySpecifyTag(SpecifyTag))
	{
		SettingItem->BindWidget(TextBlock_Name, FName("Name"));
		if (CoreWidget.GetClass()->ImplementsInterface(UVSSettingWidgetInterface::StaticClass()))
		{
			Execute_BindWidgetToSettingItem(CoreWidget, SettingItemSpecifyTag);
		}
		else if (UComboBoxString* ComboBox = Cast<UComboBoxString>(CoreWidget))
		{
			SettingItem->BindWidget(ComboBox, FName("Value"));
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
		else if (UComboBoxString* ComboBox = Cast<UComboBoxString>(CoreWidget))
		{
			SettingItem->UnbindWidget(ComboBox, FName("Value"));
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
		CoreWidget = NewObject<UWidget>(this, CoreWidgetClass);
		Panel_CoreWidget->AddChild(CoreWidget);
	}
}
