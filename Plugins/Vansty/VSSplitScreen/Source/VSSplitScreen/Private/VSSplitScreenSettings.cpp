// Copyright VanstySanic. All Rights Reserved.

#include "VSSplitScreen/Public/VSSplitScreenSettings.h"
#include "VSSplitScreen/Public/VSSplitScreenLocalPlayerController.h"

UVSSplitScreenSettings::UVSSplitScreenSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SplitScreenLocalPlayerControllerClass = AVSSplitScreenLocalPlayerController::StaticClass();
}

const UVSSplitScreenSettings* UVSSplitScreenSettings::Get()
{
	return GetDefault<UVSSplitScreenSettings>();
}
