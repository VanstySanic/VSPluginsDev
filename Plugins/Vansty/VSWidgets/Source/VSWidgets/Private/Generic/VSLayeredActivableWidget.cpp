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

void UVSLayeredActivableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const int32 WidgetNumInStackThisFrame = WidgetStack->GetNumWidgets();
	if (WidgetNumInStackLastFrame == 0 && WidgetNumInStackThisFrame > 0
		|| WidgetNumInStackLastFrame > 0 && WidgetNumInStackThisFrame == 0)
	{
		if (WidgetStack && WidgetSwitcher->GetActiveWidget() != WidgetStack && WidgetStack->GetNumWidgets() > 0)
		{
			WidgetSwitcher->SetActiveWidget(WidgetStack);
		}
		else if (Panel && WidgetSwitcher->GetActiveWidget() != Panel && (WidgetStack->GetNumWidgets() == 0 /** || !WidgetStack->GetActiveWidget() */))
		{
			WidgetSwitcher->SetActiveWidget(Panel);
		}
	}

	WidgetNumInStackLastFrame = WidgetStack->GetNumWidgets();
}
