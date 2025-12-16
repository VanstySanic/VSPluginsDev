// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"

namespace EVSSettingItem
{
	namespace Video
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(WindowMode);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScreenResolution);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(VSync);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FrameRateLimit);
	}
	
	namespace Scalability
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResolutionScale);
			
		namespace QualityLevel
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ViewDistance);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(AntiAliasing);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shadow);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(GlobalIllumination);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Reflection);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(PostProcess);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Texture);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(VisualEffect);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Foliage);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shading);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Landscape);
		}
	}

	namespace Graphics
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MotionBlur);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(AntiAliasingMethod);
	}
}