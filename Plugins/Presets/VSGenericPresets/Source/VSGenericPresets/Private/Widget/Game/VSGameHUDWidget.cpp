// Copyright VanstySanic. All Rights Reserved.

#include "Widget/Game/VSGameHUDWidget.h"
#include "Widget/Game/VSGameWidgetController.h"

UVSGameHUDWidget::UVSGameHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSGameHUDWidget::SetOwnerWidgetController(UVSGameWidgetController* InWidgetController)
{
	GameWidgetControllerPrivate = InWidgetController;
}
