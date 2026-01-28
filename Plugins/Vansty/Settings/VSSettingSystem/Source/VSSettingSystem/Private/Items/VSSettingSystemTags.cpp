// Copyright VanstySanic. All Rights Reserved.


#include "Items/VSSettingSystemTags.h"

namespace EVSSettingItem
{
	namespace Video
	{	
		UE_DEFINE_GAMEPLAY_TAG(WindowMode, "VS.SettingSystem.Item.Video.WindowMode");
		UE_DEFINE_GAMEPLAY_TAG(ScreenResolution, "VS.SettingSystem.Item.Video.ScreenResolution");
		UE_DEFINE_GAMEPLAY_TAG(VSync, "VS.SettingSystem.Item.Video.VSync");
		UE_DEFINE_GAMEPLAY_TAG(FrameRateLimit, "VS.SettingSystem.Item.Video.FrameRateLimit");
		UE_DEFINE_GAMEPLAY_TAG(Monitor, "VS.SettingSystem.Item.Video.Monitor");
	}

	namespace Scalability
	{
		UE_DEFINE_GAMEPLAY_TAG(ResolutionScale, "VS.SettingSystem.Item.Scalability.ResolutionScale");
			
		namespace QualityLevel
		{
			UE_DEFINE_GAMEPLAY_TAG(ViewDistance, "VS.SettingSystem.Item.Scalability.QualityLevel.ViewDistance");
			UE_DEFINE_GAMEPLAY_TAG(AntiAliasing, "VS.SettingSystem.Item.Scalability.QualityLevel.AntiAliasing");
			UE_DEFINE_GAMEPLAY_TAG(Shadow, "VS.SettingSystem.Item.Scalability.QualityLevel.Shadow");
			UE_DEFINE_GAMEPLAY_TAG(GlobalIllumination, "VS.SettingSystem.Item.Scalability.QualityLevel.GlobalIllumination");
			UE_DEFINE_GAMEPLAY_TAG(Reflection, "VS.SettingSystem.Item.Scalability.QualityLevel.Reflection");
			UE_DEFINE_GAMEPLAY_TAG(PostProcess, "VS.SettingSystem.Item.Scalability.QualityLevel.PostProcess");
			UE_DEFINE_GAMEPLAY_TAG(Texture, "VS.SettingSystem.Item.Scalability.QualityLevel.Texture");
			UE_DEFINE_GAMEPLAY_TAG(VisualEffect, "VS.SettingSystem.Item.Scalability.QualityLevel.VisualEffect");
			UE_DEFINE_GAMEPLAY_TAG(Foliage, "VS.SettingSystem.Item.Scalability.QualityLevel.Foliage");
			UE_DEFINE_GAMEPLAY_TAG(Shading, "VS.SettingSystem.Item.Scalability.QualityLevel.Shading");
			UE_DEFINE_GAMEPLAY_TAG(Landscape, "VS.SettingSystem.Item.Scalability.QualityLevel.Landscape");
		}
	}

	namespace Graphics
	{
		UE_DEFINE_GAMEPLAY_TAG(MotionBlur, "VS.SettingSystem.Item.Graphics.MotionBlur");
		UE_DEFINE_GAMEPLAY_TAG(AntiAliasingMethod, "VS.SettingSystem.Item.Graphics.AntiAliasingMethod");
	}

	namespace Audio
	{
		namespace Volume
		{
			UE_DEFINE_GAMEPLAY_TAG(Master, "VS.SettingSystem.Item.Audio.Volume.Master");
			UE_DEFINE_GAMEPLAY_TAG(Music, "VS.SettingSystem.Item.Audio.Volume.Music");
		}

		namespace Device
		{
			UE_DEFINE_GAMEPLAY_TAG(Output, "VS.SettingSystem.Item.Audio.Device.Output");
			UE_DEFINE_GAMEPLAY_TAG(Input, "VS.SettingSystem.Item.Audio.Device.Input");
		}
	}
}