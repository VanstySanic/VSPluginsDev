// Copyright VanstySanic. All Rights Reserved.


#include "Items/VSSettingSystemTags.h"

namespace VS
{
	namespace Settings
	{
		namespace Item
		{
			namespace Video
			{	
				UE_DEFINE_GAMEPLAY_TAG(WindowMode, "VS.Settings.Item.Video.WindowMode");
				UE_DEFINE_GAMEPLAY_TAG(ScreenResolution, "VS.Settings.Item.Video.ScreenResolution");
				UE_DEFINE_GAMEPLAY_TAG(VSync, "VS.Settings.Item.Video.VSync");
				UE_DEFINE_GAMEPLAY_TAG(FrameRateLimit, "VS.Settings.Item.Video.FrameRateLimit");
				UE_DEFINE_GAMEPLAY_TAG(Monitor, "VS.Settings.Item.Video.Monitor");
			}

			namespace Scalability
			{
				UE_DEFINE_GAMEPLAY_TAG(ResolutionScale, "VS.Settings.Item.Scalability.ResolutionScale");
					
				namespace QualityLevel
				{
					UE_DEFINE_GAMEPLAY_TAG(ViewDistance, "VS.Settings.Item.Scalability.QualityLevel.ViewDistance");
					UE_DEFINE_GAMEPLAY_TAG(AntiAliasing, "VS.Settings.Item.Scalability.QualityLevel.AntiAliasing");
					UE_DEFINE_GAMEPLAY_TAG(Shadow, "VS.Settings.Item.Scalability.QualityLevel.Shadow");
					UE_DEFINE_GAMEPLAY_TAG(GlobalIllumination, "VS.Settings.Item.Scalability.QualityLevel.GlobalIllumination");
					UE_DEFINE_GAMEPLAY_TAG(Reflection, "VS.Settings.Item.Scalability.QualityLevel.Reflection");
					UE_DEFINE_GAMEPLAY_TAG(PostProcess, "VS.Settings.Item.Scalability.QualityLevel.PostProcess");
					UE_DEFINE_GAMEPLAY_TAG(Texture, "VS.Settings.Item.Scalability.QualityLevel.Texture");
					UE_DEFINE_GAMEPLAY_TAG(VisualEffect, "VS.Settings.Item.Scalability.QualityLevel.VisualEffect");
					UE_DEFINE_GAMEPLAY_TAG(Foliage, "VS.Settings.Item.Scalability.QualityLevel.Foliage");
					UE_DEFINE_GAMEPLAY_TAG(Shading, "VS.Settings.Item.Scalability.QualityLevel.Shading");
					UE_DEFINE_GAMEPLAY_TAG(Landscape, "VS.Settings.Item.Scalability.QualityLevel.Landscape");
				}
			}

			namespace Graphics
			{
				UE_DEFINE_GAMEPLAY_TAG(MotionBlur, "VS.Settings.Item.Graphics.MotionBlur");
				UE_DEFINE_GAMEPLAY_TAG(AntiAliasingMethod, "VS.Settings.Item.Graphics.AntiAliasingMethod");
			}

			namespace Audio
			{
				namespace Volume
				{
					UE_DEFINE_GAMEPLAY_TAG(Master, "VS.Settings.Item.Audio.Volume.Master");
					UE_DEFINE_GAMEPLAY_TAG(Music, "VS.Settings.Item.Audio.Volume.Music");
				}

				namespace Device
				{
					UE_DEFINE_GAMEPLAY_TAG(Output, "VS.Settings.Item.Audio.Device.Output");
					UE_DEFINE_GAMEPLAY_TAG(Input, "VS.Settings.Item.Audio.Device.Input");
				}
			}
		}
	}
}