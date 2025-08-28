// Copyright VanstySanic. All Rights Reserved.

#include "Items/Scalability/VSSettingItem_ScalabilityQualityLevel.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_ScalabilityQualityLevel::UVSSettingItem_ScalabilityQualityLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ScalabilityQualityLevel::SetToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType)
{
	SetQualityLevel(GetQualityLevel(ValueType));
}

void UVSSettingItem_ScalabilityQualityLevel::Apply_Implementation()
{
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	Scalability::SetQualityLevels(GameUserSettings->ScalabilityQuality);
}

void UVSSettingItem_ScalabilityQualityLevel::Confirm_Implementation()
{
	LastConfirmedQualityLevel = GetQualityLevel(EVSSettingItemValueType::Current);
}

void UVSSettingItem_ScalabilityQualityLevel::Save_Implementation()
{
	Scalability::SaveState(GIsEditor ? GEditorSettingsIni : GGameUserSettingsIni);
}

bool UVSSettingItem_ScalabilityQualityLevel::EqualsToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) const
{
	return GetQualityLevel(EVSSettingItemValueType::Settings) != GetQualityLevel(ValueType);
}

void UVSSettingItem_ScalabilityQualityLevel::SetQualityLevel(int32 InQualityLevel)
{
	if (InQualityLevel == GetQualityLevel(EVSSettingItemValueType::Settings)) return;
	
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_ViewDistance)
	{
		GameUserSettings->SetViewDistanceQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_AntiAliasing)
	{
		GameUserSettings->SetAntiAliasingQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Shadow)
	{
		GameUserSettings->SetShadowQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_GlobalIllumination)
	{
		GameUserSettings->SetGlobalIlluminationQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Reflection)
	{
		GameUserSettings->SetReflectionQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_PostProcess)
	{
		GameUserSettings->SetPostProcessingQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Texture)
	{
		GameUserSettings->SetTextureQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Effects)
	{
		GameUserSettings->SetVisualEffectQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Foliage)
	{
		GameUserSettings->SetFoliageQuality(InQualityLevel);
	}
	else if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Shading)
	{
		GameUserSettings->SetShadingQuality(InQualityLevel);
	}

	NotifyUpdate();
}

int32 UVSSettingItem_ScalabilityQualityLevel::GetQualityLevel(const EVSSettingItemValueType::Type ValueType) const
{
	if (!GEngine || ValueType == EVSSettingItemValueType::None) return 0;
	if (ValueType == EVSSettingItemValueType::LastConfirmed) return LastConfirmedQualityLevel;
	
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	const Scalability::FQualityLevels& SettingsQualityLevels = GameUserSettings->ScalabilityQuality;
	const Scalability::FQualityLevels& CurrentQualityLevels = Scalability::GetQualityLevels();
	Scalability::FQualityLevels DefaultQualityLevels = CurrentQualityLevels;
	DefaultQualityLevels.SetDefaults();
	
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_ViewDistance)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.ViewDistanceQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.ViewDistanceQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.ViewDistanceQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_AntiAliasing)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.AntiAliasingQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.AntiAliasingQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.AntiAliasingQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Shadow)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.ShadowQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.ShadowQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.ShadowQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_GlobalIllumination)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.GlobalIlluminationQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.GlobalIlluminationQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.GlobalIlluminationQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Reflection)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.ReflectionQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.ReflectionQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.ReflectionQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_PostProcess)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.PostProcessQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.PostProcessQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.PostProcessQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Texture)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.TextureQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.TextureQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.TextureQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Effects)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.EffectsQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.EffectsQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.EffectsQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Foliage)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.FoliageQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.FoliageQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.FoliageQuality;
			
		default:
			return 0;
		}
	}
	if (IdentityTag == EVSSettingItem::Scalability_QualityLevel_Shading)
	{
		switch (ValueType)
		{
		case EVSSettingItemValueType::Default:
			return DefaultQualityLevels.ShadingQuality;
			
		case EVSSettingItemValueType::Current:
			return CurrentQualityLevels.ShadingQuality;
			
		case EVSSettingItemValueType::Settings:
			return SettingsQualityLevels.ShadingQuality;
			
		default:
			return 0;
		}
	}

	return 0;
}