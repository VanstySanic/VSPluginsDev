// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSSettingSystemTags.h"

namespace EVSSettingItem
{
	UE_DEFINE_GAMEPLAY_TAG(Internationalization_LanguageAndLocale, "VSSettingSystem.SettingItem.Internationalization.LanguageAndLocale");
	UE_DEFINE_GAMEPLAY_TAG(Internationalization_AssetGroupCultures, "VSSettingSystem.SettingItem.Internationalization.AssetGroupCultures");

	UE_DEFINE_GAMEPLAY_TAG(Video_ScreenResolution, "VSSettingSystem.SettingItem.Video.ScreenResolution");
	UE_DEFINE_GAMEPLAY_TAG(Video_WindowMode, "VSSettingSystem.SettingItem.Video.WindowMode");
	UE_DEFINE_GAMEPLAY_TAG(Video_FrameRateLimit, "VSSettingSystem.SettingItem.Video.FrameRateLimit");
	UE_DEFINE_GAMEPLAY_TAG(Video_VSync, "VSSettingSystem.SettingItem.Video.VSync");

	UE_DEFINE_GAMEPLAY_TAG(Scalability_ResolutionScale, "VSSettingSystem.SettingItem.Scalability.ResolutionScale");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_ViewDistance, "VSSettingSystem.SettingItem.Scalability.QualityLevel.ViewDistance");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_AntiAliasing, "VSSettingSystem.SettingItem.Scalability.QualityLevel.AntiAliasing");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_Shadow, "VSSettingSystem.SettingItem.Scalability.QualityLevel.Shadow");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_GlobalIllumination, "VSSettingSystem.SettingItem.Scalability.QualityLevel.GlobalIllumination");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_Reflection, "VSSettingSystem.SettingItem.Scalability.Quality.Reflection");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_PostProcess, "VSSettingSystem.SettingItem.Scalability.QualityLevel.PostProcess");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_Texture, "VSSettingSystem.SettingItem.Scalability.QualityLevel.Texture");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_Effects, "VSSettingSystem.SettingItem.Scalability.QualityLevel.Effects");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_Foliage, "VSSettingSystem.SettingItem.Scalability.QualityLevel.Foliage");
	UE_DEFINE_GAMEPLAY_TAG(Scalability_QualityLevel_Shading, "VSSettingSystem.SettingItem.Scalability.QualityLevel.Shading");

	UE_DEFINE_GAMEPLAY_TAG(Graphics_MotionBlur, "VSSettingSystem.SettingItem.Graphics.MotionBlur");

	UE_DEFINE_GAMEPLAY_TAG(Audio_SoundMixClassVolume_Primary, "VSSettingSystem.SettingItem.Audio.SoundMixClassVolume.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Audio_SoundMixClassVolume_Music, "VSSettingSystem.SettingItem.Audio.SoundMixClassVolume.Music");
}

namespace EVSSettingItemCategory
{
	UE_DEFINE_GAMEPLAY_TAG(Internationalization, "VSSettingSystem.ItemCategory.Internationalization");
	UE_DEFINE_GAMEPLAY_TAG(Scalability, "VSSettingSystem.ItemCategory.Scalability");
	UE_DEFINE_GAMEPLAY_TAG(Graphics, "VSSettingSystem.ItemCategory.Graphics");
	UE_DEFINE_GAMEPLAY_TAG(Audio, "VSSettingSystem.ItemCategory.Audio");
}