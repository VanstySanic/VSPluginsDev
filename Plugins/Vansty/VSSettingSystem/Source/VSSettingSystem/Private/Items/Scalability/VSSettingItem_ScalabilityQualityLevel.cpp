// Copyright VanstySanic. All Rights Reserved.

#include "Items/Scalability/VSSettingItem_ScalabilityQualityLevel.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_ScalabilityQualityLevel::UVSSettingItem_ScalabilityQualityLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Scalability_QualityLevel_ViewDistance);
	QualityLevelNames = TMap<int32, FText>
		{
			{ 0, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.0", "Low") },
			{ 1, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.1", "Medium") },
			{ 2, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.2", "High") },
			{ 3, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.3", "Epic") },
			{ 4, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.4", "Cinematic") },
		};
}

#if WITH_EDITOR
void UVSSettingItem_ScalabilityQualityLevel::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_ScalabilityQualityLevel, ItemInfo))
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FVSSettingItemInfo, SpecifyTag))
		{
			bool bShouldUpdateDefaultName = ItemInfo.DisplayName.IsEmpty() || FName(ItemInfo.DisplayName.ToString()).IsNone();
			if (!bShouldUpdateDefaultName)
			{
				for (const auto& DefaultScalabilityQualityLevelText : GetDefaultScalabilityQualityLevelNames())
				{
					if (DefaultScalabilityQualityLevelText.Value.EqualTo(ItemInfo.DisplayName))
					{
						bShouldUpdateDefaultName = true;
						break;
					}
				}
			}
			
			if (bShouldUpdateDefaultName)
			{
				if (GetDefaultScalabilityQualityLevelNames().Contains(ItemInfo.SpecifyTag))
				{
					ItemInfo.DisplayName = GetDefaultScalabilityQualityLevelNames().FindRef(ItemInfo.SpecifyTag);
				}
				else
				{
					ItemInfo.DisplayName = FText::FromString("None");
				}
			}
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_ScalabilityQualityLevel::Apply_Implementation()
{
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	Scalability::SetQualityLevels(GameUserSettings->ScalabilityQuality);
}

void UVSSettingItem_ScalabilityQualityLevel::Confirm_Implementation()
{
	LastConfirmedQualityLevel = GetQualityLevel(EVSSettingItemValueSource::Current);
}

void UVSSettingItem_ScalabilityQualityLevel::Save_Implementation()
{
	Scalability::SaveState(GIsEditor ? GEditorSettingsIni : GGameUserSettingsIni);
}

void UVSSettingItem_ScalabilityQualityLevel::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetQualityLevel(GetQualityLevel(ValueSource));
}

bool UVSSettingItem_ScalabilityQualityLevel::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetQualityLevel(EVSSettingItemValueSource::Settings) != GetQualityLevel(ValueSource);
}

void UVSSettingItem_ScalabilityQualityLevel::SetQualityLevel(int32 InQualityLevel)
{
	if (InQualityLevel == GetQualityLevel(EVSSettingItemValueSource::Settings)) return;
	
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_ViewDistance)
	{
		GameUserSettings->SetViewDistanceQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_AntiAliasing)
	{
		GameUserSettings->SetAntiAliasingQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Shadow)
	{
		GameUserSettings->SetShadowQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_GlobalIllumination)
	{
		GameUserSettings->SetGlobalIlluminationQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Reflection)
	{
		GameUserSettings->SetReflectionQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_PostProcess)
	{
		GameUserSettings->SetPostProcessingQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Texture)
	{
		GameUserSettings->SetTextureQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Effects)
	{
		GameUserSettings->SetVisualEffectQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Foliage)
	{
		GameUserSettings->SetFoliageQuality(InQualityLevel);
	}
	else if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Shading)
	{
		GameUserSettings->SetShadingQuality(InQualityLevel);
	}

	NotifyUpdate();
}

int32 UVSSettingItem_ScalabilityQualityLevel::GetQualityLevel(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine || ValueSource == EVSSettingItemValueSource::None) return 0;
	if (ValueSource == EVSSettingItemValueSource::LastConfirmed) return LastConfirmedQualityLevel;
	
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	const Scalability::FQualityLevels& SettingsQualityLevels = GameUserSettings->ScalabilityQuality;
	const Scalability::FQualityLevels& CurrentQualityLevels = Scalability::GetQualityLevels();
	Scalability::FQualityLevels DefaultQualityLevels = CurrentQualityLevels;
	DefaultQualityLevels.SetDefaults();
	
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_ViewDistance)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.ViewDistanceQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.ViewDistanceQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.ViewDistanceQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_AntiAliasing)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.AntiAliasingQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.AntiAliasingQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.AntiAliasingQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Shadow)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.ShadowQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.ShadowQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.ShadowQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_GlobalIllumination)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.GlobalIlluminationQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.GlobalIlluminationQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.GlobalIlluminationQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Reflection)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.ReflectionQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.ReflectionQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.ReflectionQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_PostProcess)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.PostProcessQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.PostProcessQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.PostProcessQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Texture)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.TextureQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.TextureQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.TextureQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Effects)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.EffectsQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.EffectsQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.EffectsQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Foliage)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.FoliageQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.FoliageQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.FoliageQuality;
			
		default:
			return 0;
		}
	}
	if (ItemInfo.SpecifyTag == EVSSettingItem::Scalability_QualityLevel_Shading)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return DefaultQualityLevels.ShadingQuality;
			
		case EVSSettingItemValueSource::Current:
			return CurrentQualityLevels.ShadingQuality;
			
		case EVSSettingItemValueSource::Settings:
			return SettingsQualityLevels.ShadingQuality;
			
		default:
			return 0;
		}
	}

	return 0;
}

const TMap<FGameplayTag, FText>& UVSSettingItem_ScalabilityQualityLevel::GetDefaultScalabilityQualityLevelNames()
{
	static TMap<FGameplayTag, FText> ScalabilityQualityLevel = TMap<FGameplayTag, FText>
	{
		{ EVSSettingItem::Scalability_QualityLevel_ViewDistance, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.ViewDistance", "View Distance") },
		{ EVSSettingItem::Scalability_QualityLevel_AntiAliasing, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.AntiAliasing", "Anti-Aliasing") },
		{ EVSSettingItem::Scalability_QualityLevel_Shadow, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.Shadow", "Shadow") },
		{ EVSSettingItem::Scalability_QualityLevel_GlobalIllumination, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.GlobalIllumination", "Global Illumination") },
		{ EVSSettingItem::Scalability_QualityLevel_Reflection, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.Reflection", "Reflection") },
		{ EVSSettingItem::Scalability_QualityLevel_PostProcess, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.PostProcess", "Post-Process") },
		{ EVSSettingItem::Scalability_QualityLevel_Texture, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.Texture", "Texture") },
		{ EVSSettingItem::Scalability_QualityLevel_Effects, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.PostProcess", "Post-Process") },
		{ EVSSettingItem::Scalability_QualityLevel_Foliage, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.Effects", "Effects") },
		{ EVSSettingItem::Scalability_QualityLevel_Shading, NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.QualityLevel.Shading", "Shading") }
	};

	return ScalabilityQualityLevel;
}
