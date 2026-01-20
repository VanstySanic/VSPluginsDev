// Copyright VanstySanic. All Rights Reserved.

#include "Items/Graphics/VSSettingItem_ScalabilityQualityLevel.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

UVSSettingItem_ScalabilityQualityLevel::UVSSettingItem_ScalabilityQualityLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::Integer);
	ConfigParams.ConfigFileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigParams.ConfigSection = FString("ScalabilityGroups");
	ItemTag = UGameplayTagsManager::Get().RequestGameplayTag("VS.SettingSystem.Item.Scalability.QualityLevel");
}

#if WITH_EDITOR
void UVSSettingItem_ScalabilityQualityLevel::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_ScalabilityQualityLevel, ItemTag))
	{
		SetConsoleVariableName(GetScalabilityQualityLevelConsoleVariableNames().FindRef(ItemTag));
		ConfigParams.ConfigKeyName = GetScalabilityQualityLevelConsoleVariableNames().FindRef(ItemTag);

		bool bShouldAssignName = ItemInfo.DisplayName.IsEmpty();
		if (!bShouldAssignName)
		{
			for (const TPair<FGameplayTag, FText>& ScalabilityQualityLevelDefaultDisplayName : GetScalabilityQualityLevelDefaultDisplayNames())
			{
				if (ScalabilityQualityLevelDefaultDisplayName.Value.EqualTo(ItemInfo.DisplayName))
				{
					bShouldAssignName = true;
					break;
				}
			}
		}
		
		if (bShouldAssignName)
		{
			ItemInfo.DisplayName = GetScalabilityQualityLevelDefaultDisplayNames().FindRef(ItemTag);
		}
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_ScalabilityQualityLevel, EditorPreviewQualityLevel))
	{
		SetIntegerValue(static_cast<int32>(EditorPreviewQualityLevel));
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_ScalabilityQualityLevel::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();

#if WITH_EDITORONLY_DATA
	EditorPreviewQualityLevel = GetQualityLevel(EVSSettingItemValueSource::System);
#endif
	
	if (!GEngine || !GEngine->GameUserSettings) return;

	/** Sync to GameUserSettings. */
	const int32 QualityLevel = GetIntegerValue(EVSSettingItemValueSource::System);
	if (ItemTag == EVSSettingItem::Scalability::QualityLevel::AntiAliasing)
	{
		GEngine->GameUserSettings->SetAntiAliasingQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::VisualEffect)
	{
		GEngine->GameUserSettings->SetVisualEffectQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::Foliage)
	{
		GEngine->GameUserSettings->SetFoliageQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::GlobalIllumination)
	{
		GEngine->GameUserSettings->SetGlobalIlluminationQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::PostProcess)
	{
		GEngine->GameUserSettings->SetPostProcessingQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::Reflection)
	{
		GEngine->GameUserSettings->SetReflectionQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::Shading)
	{
		GEngine->GameUserSettings->SetShadingQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::Shadow)
	{
		GEngine->GameUserSettings->SetShadowQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::Texture)
	{
		GEngine->GameUserSettings->SetTextureQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::ViewDistance)
	{
		GEngine->GameUserSettings->SetViewDistanceQuality(QualityLevel);
	}
	else if (ItemTag == EVSSettingItem::Scalability::QualityLevel::Landscape)
	{
		/** Do nothing. */
	}
}

bool UVSSettingItem_ScalabilityQualityLevel::IsValueValid_Implementation() const
{
	const int32 CurrentQualityLevel = GetIntegerValue(EVSSettingItemValueSource::System);
	return FVSMath::IsInRange(CurrentQualityLevel, 0, 4);
}

void UVSSettingItem_ScalabilityQualityLevel::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	
	const int32 CurrentQualityLevel = GetIntegerValue(EVSSettingItemValueSource::System);
	SetIntegerValue(FMath::Clamp(CurrentQualityLevel, 0, 4));
}

int32 UVSSettingItem_ScalabilityQualityLevel::GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return Super::GetIntegerValue_Implementation(ValueSource);

	if (ValueType == EVSCommonSettingValueType::Integer)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return Scalability::DefaultQualityLevel;
			
		default: ;
		}
	}
		
	return Super::GetIntegerValue_Implementation(ValueSource);
}

FText UVSSettingItem_ScalabilityQualityLevel::ValueStringToText_Implementation(const FString& String) const
{
	static TMap<FString, FText> OutMap = TMap<FString, FText>
	{
		{ "0", Scalability::GetQualityLevelText(0, 5) },
		{ "1", Scalability::GetQualityLevelText(1, 5) },
		{ "2", Scalability::GetQualityLevelText(2, 5) },
		{ "3", Scalability::GetQualityLevelText(3, 5) },
		{ "4", Scalability::GetQualityLevelText(4, 5) },
	};
	
	return OutMap.FindRef(String);
}

#if WITH_EDITOR
void UVSSettingItem_ScalabilityQualityLevel::EditorPostInitialized_Implementation()
{
	Super::EditorPostInitialized_Implementation();

	EditorPreviewQualityLevel = GetQualityLevel(EVSSettingItemValueSource::System);
}
#endif

void UVSSettingItem_ScalabilityQualityLevel::SetQualityLevel(EPerQualityLevels InQualityLevel)
{
	SetIntegerValue(static_cast<int32>(InQualityLevel));
}

EPerQualityLevels UVSSettingItem_ScalabilityQualityLevel::GetQualityLevel(const EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<EPerQualityLevels>(GetIntegerValue(ValueSource));
}

#if WITH_EDITOR
TMap<FGameplayTag, FText> UVSSettingItem_ScalabilityQualityLevel::GetScalabilityQualityLevelDefaultDisplayNames()
{
	static TMap<FGameplayTag, FText> ScalabilityQualityLevels = TMap<FGameplayTag, FText>
	{
		{ EVSSettingItem::Scalability::QualityLevel::ViewDistance, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.ViewDistance", "View Distance Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::AntiAliasing, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.AntiAliasing", "Anti-Aliasing Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::Shadow, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Shadow", "Shadow Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::GlobalIllumination, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.GlobalIllumination", "Global Illumination Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::Reflection, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Reflection", "Reflection Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::PostProcess, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.PostProcess", "Post-Process Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::Texture, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Texture", "Texture Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::VisualEffect, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Effects", "Effects Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::Foliage, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Foliage", "Foliage Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::Shading, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Shading", "Shading Quality") },
		{ EVSSettingItem::Scalability::QualityLevel::Landscape, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Landscape", "Landscape Quality") }
	};

	return ScalabilityQualityLevels;
}

TMap<FGameplayTag, FString> UVSSettingItem_ScalabilityQualityLevel::GetScalabilityQualityLevelConsoleVariableNames()
{
	static TMap<FGameplayTag, FString> ScalabilityQualityLevels = TMap<FGameplayTag, FString>
	{
		{ EVSSettingItem::Scalability::QualityLevel::ViewDistance, FString("sg.ViewDistanceQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::AntiAliasing, FString("sg.AntiAliasingQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::Shadow, FString("sg.ShadowQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::GlobalIllumination, FString("sg.GlobalIlluminationQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::Reflection, FString("sg.ReflectionQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::PostProcess, FString("sg.PostProcessQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::Texture, FString("sg.TextureQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::VisualEffect, FString("sg.EffectsQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::Foliage, FString("sg.FoliageQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::Shading, FString("sg.ShadingQuality") },
		{ EVSSettingItem::Scalability::QualityLevel::Landscape, FString("sg.LandscapeQuality") }
	};

	return ScalabilityQualityLevels;
}
#endif