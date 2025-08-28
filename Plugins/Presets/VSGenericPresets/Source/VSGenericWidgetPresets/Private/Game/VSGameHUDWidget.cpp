// Copyright VanstySanic. All Rights Reserved.


#include "VSGenericWidgetPresets/Public/Game/VSGameHUDWidget.h"
#include "Game/VSGameWidgetController.h"

UVSGameHUDWidget::UVSGameHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSGameHUDWidget::SetOwnerWidgetController(UVSGameWidgetController* InWidgetController)
{
	GameWidgetControllerPrivate = InWidgetController;
}
