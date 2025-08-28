// Copyright VanstySanic. All Rights Reserved.

#include "Generic/VSLayeredActivableWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UVSLayeredActivableWidget::UVSLayeredActivableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSLayeredActivableWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	if (!WidgetSwitcher) return false;
	if (Panel && !WidgetSwitcher->HasChild(Panel)) return false;
	if (WidgetStack && !WidgetSwitcher->HasChild(WidgetStack)) return false;

	return true;
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
