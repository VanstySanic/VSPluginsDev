// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
// #include "VSInteractSystemTags.generated.h"

namespace EVSInteractState
{
	VSINTERACTSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interacting);
}

namespace EVSInteractEvent
{
	VSINTERACTSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interact_Start);
	VSINTERACTSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interact_End);
}
