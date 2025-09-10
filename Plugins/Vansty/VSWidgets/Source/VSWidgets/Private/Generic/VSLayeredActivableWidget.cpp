// Copyright VanstySanic. All Rights Reserved.

#include "Generic/VSLayeredActivableWidget.h"
#include "VSWidgetLibrary.h"
#include "Components/WidgetSwitcher.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UVSLayeredActivableWidget::UVSLayeredActivableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSLayeredActivableWidget::NativeDestruct()
{
	if (IsActivated())
	{
		DeactivateWidget();
	}
	
	Super::NativeDestruct();
}

void UVSLayeredActivableWidget::SwitchToPanel()
{
	if (WidgetSwitcher && Panel)
	{
		WidgetSwitcher->SetActiveWidget(Panel);
	}
}

void UVSLayeredActivableWidget::SwitchToWidgetStack()
{
	if (WidgetSwitcher && WidgetStack)
	{
		WidgetSwitcher->SetActiveWidget(WidgetStack);
	}
}

void UVSLayeredActivableWidget::NativeOnActivated()
{
	/** Cache the ui input config befor set to new. */
	if (bRestoreActiveUIInputConfig)
	{
		CachedUIInputConfig = UVSWidgetLibrary::GetCurrentUIInputConfig(GetOwningPlayer());
	}

	Super::NativeOnActivated();
}

void UVSLayeredActivableWidget::NativeOnDeactivated()
{
	if (bRestoreActiveUIInputConfig)
	{
		UVSWidgetLibrary::SetCurrentUIInputConfig(GetOwningPlayer(), CachedUIInputConfig);
	}
	
	Super::NativeOnDeactivated();
}
