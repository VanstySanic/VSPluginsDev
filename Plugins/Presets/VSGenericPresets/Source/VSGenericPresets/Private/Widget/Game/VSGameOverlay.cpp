// Copyright VanstySanic. All Rights Reserved.

#include "Widget/Game/VSGameOverlay.h"
#include "Components/VSNotifyLine.h"
#include "Components/VSSubtitleLine.h"
#include "Widget/Game/VSGameWidgetController.h"

UVSGameOverlay::UVSGameOverlay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TMap<FName, UPanelWidget*> UVSGameOverlay::GetTypedSubtitlePanelMap_Implementation() const
{
	return TMap<FName, UPanelWidget*>();
}

TMap<FName, UPanelWidget*> UVSGameOverlay::GetTypedNotifyPanelMap_Implementation() const
{
	return TMap<FName, UPanelWidget*>();
}

void UVSGameOverlay::SetOwnerWidgetController(UVSGameWidgetController* InWidgetController)
{
	GameWidgetControllerPrivate = InWidgetController;
}

void UVSGameOverlay::DisplaySubtitle(const FVSWidgetSubtitleParams& SubtitleParams, FName TypeID)
{
	const auto& PanelMap = GetTypedSubtitlePanelMap();
	if (!PanelMap.Contains(TypeID) || !PanelMap[TypeID]) return;
	if (!SubtitleLineClass.Contains(TypeID) || !SubtitleLineClass[TypeID]) return;

	UVSSubtitleLine::CreateAndDisplay(PanelMap[TypeID], SubtitleLineClass[TypeID], SubtitleParams);
}

void UVSGameOverlay::DisplayNotify(const FVSWidgetNotifyParams& NotifyParams, FName TypeID)
{
	const auto& PanelMap = GetTypedNotifyPanelMap();
	if (!PanelMap.Contains(TypeID) || !PanelMap[TypeID]) return;
	if (!NotifyLineClasses.Contains(TypeID) || !NotifyLineClasses[TypeID]) return;

	UVSNotifyLine::CreateAndDisplay(PanelMap[TypeID], NotifyLineClasses[TypeID], NotifyParams);
}