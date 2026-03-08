// Copyright VanstySanic. All Rights Reserved.

#include "Items/Graphics/VSSettingItem_ScalabilityQualityLevel.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

UVSSettingItem_ScalabilityQualityLevel::UVSSettingItem_ScalabilityQualityLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::Integer);
	
	ItemIdentifier = VS::Settings::Item::Scalability::QualityLevel::AntiAliasing;
	ItemInfo.ItemTags.AddTag(VS::Settings::Item::Scalability::QualityLevel::AntiAliasing.GetTag().RequestDirectParent());
	ConfigSettings.FileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigSettings.Section = FString("ScalabilityGroups");
}

#if WITH_EDITOR
void UVSSettingItem_ScalabilityQualityLevel::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_ScalabilityQualityLevel, ItemIdentifier))
	{
		SetConsoleVariableName(GetScalabilityQualityLevelConsoleVariableNames().FindRef(ItemIdentifier));
		ConfigSettings.PrimaryKey = GetScalabilityQualityLevelConsoleVariableNames().FindRef(ItemIdentifier);

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
			ItemInfo.DisplayName = GetScalabilityQualityLevelDefaultDisplayNames().FindRef(ItemIdentifier);
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
	
	if (!GEngine || !GEngine->GetGameUserSettings()) return;

	/** Sync to GameUserSettings. */
	const int32 QualityLevel = GetIntegerValue(EVSSettingItemValueSource::System);
	if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::AntiAliasing)
	{
		GEngine->GameUserSettings->SetAntiAliasingQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::VisualEffect)
	{
		GEngine->GameUserSettings->SetVisualEffectQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::Foliage)
	{
		GEngine->GameUserSettings->SetFoliageQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::GlobalIllumination)
	{
		GEngine->GameUserSettings->SetGlobalIlluminationQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::PostProcess)
	{
		GEngine->GameUserSettings->SetPostProcessingQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::Reflection)
	{
		GEngine->GameUserSettings->SetReflectionQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::Shading)
	{
		GEngine->GameUserSettings->SetShadingQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::Shadow)
	{
		GEngine->GameUserSettings->SetShadowQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::Texture)
	{
		GEngine->GameUserSettings->SetTextureQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::ViewDistance)
	{
		GEngine->GameUserSettings->SetViewDistanceQuality(QualityLevel);
	}
	else if (ItemIdentifier == VS::Settings::Item::Scalability::QualityLevel::Landscape)
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
		{ VS::Settings::Item::Scalability::QualityLevel::ViewDistance, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.ViewDistance", "View Distance Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::AntiAliasing, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.AntiAliasing", "Anti-Aliasing Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Shadow, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Shadow", "Shadow Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::GlobalIllumination, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.GlobalIllumination", "Global Illumination Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Reflection, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Reflection", "Reflection Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::PostProcess, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.PostProcess", "Post-Process Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Texture, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Texture", "Texture Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::VisualEffect, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Effects", "Effects Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Foliage, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Foliage", "Foliage Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Shading, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Shading", "Shading Quality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Landscape, NSLOCTEXT("VS.SettingSystem.Item.Scalability.QualityLevel", "DisplayName.Landscape", "Landscape Quality") }
	};

	return ScalabilityQualityLevels;
}

TMap<FGameplayTag, FString> UVSSettingItem_ScalabilityQualityLevel::GetScalabilityQualityLevelConsoleVariableNames()
{
	static TMap<FGameplayTag, FString> ScalabilityQualityLevels = TMap<FGameplayTag, FString>
	{
		{ VS::Settings::Item::Scalability::QualityLevel::ViewDistance, FString("sg.ViewDistanceQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::AntiAliasing, FString("sg.AntiAliasingQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Shadow, FString("sg.ShadowQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::GlobalIllumination, FString("sg.GlobalIlluminationQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Reflection, FString("sg.ReflectionQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::PostProcess, FString("sg.PostProcessQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Texture, FString("sg.TextureQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::VisualEffect, FString("sg.EffectsQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Foliage, FString("sg.FoliageQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Shading, FString("sg.ShadingQuality") },
		{ VS::Settings::Item::Scalability::QualityLevel::Landscape, FString("sg.LandscapeQuality") }
	};

	return ScalabilityQualityLevels;
}
#endif