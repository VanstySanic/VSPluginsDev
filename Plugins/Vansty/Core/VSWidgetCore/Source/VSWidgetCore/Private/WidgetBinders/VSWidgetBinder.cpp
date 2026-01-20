// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSWidgetBinder.h"
#include "VSWidgetController.h"
#include "Components/Widget.h"

UVSWidgetBinder::UVSWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WidgetControllerPrivate = GetTypedOuter<UVSWidgetController>();
}

void UVSWidgetBinder::Initialize_Implementation()
{
	
}

void UVSWidgetBinder::Uninitialize_Implementation()
{
	
}

void UVSWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	
}

void UVSWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	
}

void UVSWidgetBinder::UpdateBinder_Implementation(float DeltaTime)
{
	
}

UWidget* UVSWidgetBinder::GetBoundTypedWidget(const FName TypeName) const
{
	if (UVSWidgetController* WidgetController = Cast<UVSWidgetController>(WidgetControllerPrivate.Get()))
	{
		if (UWidget* Widget = WidgetController->GetBoundTypedWidget(TypeName))
		{
			return Widget;
		}
	}

	return nullptr;
}

void UVSWidgetBinder::RebindWidgetByType(FName TypeName)
{
	if (UWidget* Widget = GetBoundTypedWidget(TypeName))
	{
		UnbindTypedWidget(TypeName, Widget);
		BindTypedWidget(TypeName, Widget);
	}
}

void UVSWidgetBinder::RebindAllWidgets()
{
	if (UVSWidgetController* WidgetController = Cast<UVSWidgetController>(WidgetControllerPrivate.Get()))
	{
		for (const TPair<FName, UWidget*>& BoundTypedWidget : WidgetController->GetAllBoundTypedWidgets())
		{
			UnbindTypedWidget(BoundTypedWidget.Key, BoundTypedWidget.Value);
		}

		for (const TPair<FName, UWidget*>& BoundTypedWidget : WidgetController->GetAllBoundTypedWidgets())
		{
			BindTypedWidget(BoundTypedWidget.Key, BoundTypedWidget.Value);
		}
	}
}
