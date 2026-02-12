// Copyright VanstySanic. All Rights Reserved.

#include "Widgets/VSSettingItemWidget.h"
#include "CommonRotator.h"
#include "Blueprint/WidgetTree.h"
#include "Mediator/VSSettingItemWidgetController.h"

UVSSettingItemWidget::UVSSettingItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SettingWidgetController = CreateDefaultSubobject<UVSSettingItemWidgetController>(TEXT("SettingsWidgetController"));
}

void UVSSettingItemWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (SettingWidgetController)
	{
		SettingWidgetController->UnbindWidget(this, FName("Item"));
		SettingWidgetController->Unregister();

		RefreshCoreWidget();

		SettingWidgetController->Register();
		SettingWidgetController->BindWidget(this, FName("Item"));
	}
}

TMap<UWidget*, FName> UVSSettingItemWidget::GetDesiredBoundTypedWidgets_Implementation() const
{
	TMap<UWidget*, FName> OutMap;
	
	OutMap.Add(TextBlock_Name, FName("Name"));
	OutMap.Add(CoreWidget, FName("Core"));
	
	return OutMap;
}

void UVSSettingItemWidget::RefreshCoreWidget()
{
	if (CoreWidget)
	{
		if (SettingWidgetController)
		{
			SettingWidgetController->UnbindWidget(CoreWidget, FName("Core"));
		}

		CoreWidget->RemoveFromParent();
		WidgetTree->RemoveWidget(CoreWidget);
	}

	if (!CoreWidgetClass) return;

	CoreWidget = WidgetTree->ConstructWidget(CoreWidgetClass);
	if (CoreWidget)
	{
		if (Panel_CoreWidget)
		{
			Panel_CoreWidget->AddChild(CoreWidget);
			CoreWidgetPanelSlotSettings.ApplyToWidget(CoreWidget);
		}

		if (SettingWidgetController)
		{
			SettingWidgetController->BindWidget(CoreWidget, FName("Core"));
		}
	}
}
