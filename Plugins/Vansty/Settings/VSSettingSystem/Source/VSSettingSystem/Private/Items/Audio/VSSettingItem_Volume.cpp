// Copyright VanstySanic. All Rights Reserved.

#include "Items/Audio/VSSettingItem_Volume.h"
#include "AudioDevice.h"
#include "AudioDeviceManager.h"
#include "VSSettingSystemConfig.h"
#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Items/VSSettingSystemTags.h"
#include "Sound/SoundMix.h"

UVSSettingItem_Volume::UVSSettingItem_Volume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	ConfigSettings.FileName = "GameUserSettings";
	ConfigSettings.Section = "VS.Settings.Item.Audio.Volume";

	DisplayTextFormat = FText::FromString(TEXT("{0}%"));
	DisplayFractionDigitRange = FIntPoint(0, 0);
	DisplayValueMultiplier = 100.f;
	DisplayMutedText = UVSSettingSystemConfig::Get()->VolumeMutedText;
	
	InternalChangeActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
		EVSSettingItemAction::Save,
	};
	
#if WITH_EDITOR
	EditorChangeActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
	};
#endif
}

#if WITH_EDITOR
void UVSSettingItem_Volume::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_Volume, ItemTag))
	{
		bool bShouldAssignName = ItemInfo.DisplayName.IsEmpty();
		if (!bShouldAssignName)
		{
			for (const TPair<FGameplayTag, FText>& DisplayName : GetVolumeTypeDisplayNames())
			{
				if (DisplayName.Value.EqualTo(ItemInfo.DisplayName))
				{
					bShouldAssignName = true;
					break;
				}
			}
		}
		if (bShouldAssignName)
		{
			ItemInfo.DisplayName = GetVolumeTypeDisplayNames().FindRef(ItemTag);
		}

		bool bShouldAssignConfigKey = FName(ConfigSettings.PrimaryKey).IsNone();
		if (bShouldAssignConfigKey)
		{
			for (const TPair<FGameplayTag, FString>& VolumeTypeConfigKeyName : GetVolumeTypeConfigKeyNames())
			{
				if (VolumeTypeConfigKeyName.Key == ItemTag)
				{
					ConfigSettings.PrimaryKey = GetVolumeTypeConfigKeyNames().FindRef(ItemTag);
					ConfigSettings.AdditionalNamedKeys.FindOrAdd("Muted", ConfigSettings.PrimaryKey + ".Muted");
				}
			}
		}
	}
}
#endif

bool UVSSettingItem_Volume::IsValueValid_Implementation() const
{
	if (!Super::IsValueValid_Implementation()) return false;

	const float NonMutedVolume = GetNonMutedVolume(EVSSettingItemValueSource::System);
	if (NonMutedVolume < 0.f) return false;
	
	return true;
}

void UVSSettingItem_Volume::Validate_Implementation()
{
	Super::Validate_Implementation();

	float NonMutedVolume = GetNonMutedVolume(EVSSettingItemValueSource::System);
	if (NonMutedVolume < 0.f)
	{
		NonMutedVolume = 0.f;
		SetValue(NonMutedVolume);
	}
}

void UVSSettingItem_Volume::Apply_Implementation()
{
	Super::Apply_Implementation();

	if (!SoundMix || !SoundClass || !FAudioDeviceManager::Get() || !FAudioDeviceManager::Get()->GetActiveAudioDevice()) return;

	FAudioDeviceManager::Get()->GetActiveAudioDevice()->PushSoundMixModifier(SoundMix);
	const FSoundClassAdjuster SoundClassAdjuster = UVSPlatformLibrary::GetActiveSoundClassAdjuster(SoundMix, SoundClass);
	FAudioDeviceManager::Get()->GetActiveAudioDevice()->SetSoundMixClassOverride(SoundMix, SoundClass, GetVolume(EVSSettingItemValueSource::System),
		SoundClassAdjuster.PitchAdjuster, SoundMix->FadeInTime, SoundClassAdjuster.bApplyToChildren);
}

float UVSSettingItem_Volume::GetNonMutedValue_Implementation(EVSSettingItemValueSource::Type Source) const
{
	if (!SoundMix || !SoundClass) return Super::GetNonMutedValue_Implementation(Source);
	
	switch (Source)
	{
	case EVSSettingItemValueSource::Default:
		for (const FSoundClassAdjuster& SoundClassEffect : SoundMix->SoundClassEffects)
		{
			if (SoundClassEffect.SoundClassObject == SoundClass)
			{
				return SoundClassEffect.VolumeAdjuster;
			}
		}
		return 1.f;
		
	case EVSSettingItemValueSource::Game:
		return UVSPlatformLibrary::GetActiveSoundClassAdjuster(SoundMix, SoundClass).VolumeAdjuster;
		
	default: ;
	}
	
	return Super::GetNonMutedValue_Implementation(Source);
}

bool UVSSettingItem_Volume::GetIsMuted_Implementation(EVSSettingItemValueSource::Type Source) const
{
	if (!SoundMix || !SoundClass) return Super::GetIsMuted_Implementation(Source);

	switch (Source)
	{
	case EVSSettingItemValueSource::Default:
		for (const FSoundClassAdjuster& SoundClassEffect : SoundMix->SoundClassEffects)
		{
			if (SoundClassEffect.SoundClassObject == SoundClass)
			{
				return FMath::IsNearlyZero(SoundClassEffect.VolumeAdjuster);
			}
		}
		return false;
		
	case EVSSettingItemValueSource::Game:
		return FMath::IsNearlyZero(UVSPlatformLibrary::GetActiveSoundClassAdjuster(SoundMix, SoundClass).VolumeAdjuster);
		
	default: ;
	}
	
	return Super::GetIsMuted_Implementation(Source);
}

float UVSSettingItem_Volume::GetVolume(EVSSettingItemValueSource::Type Source) const
{
	return GetValue(Source);
}

float UVSSettingItem_Volume::GetNonMutedVolume(EVSSettingItemValueSource::Type Source) const
{
	return GetNonMutedValue(Source);
}

#if WITH_EDITOR
TMap<FGameplayTag, FText> UVSSettingItem_Volume::GetVolumeTypeDisplayNames()
{
	static TMap<FGameplayTag, FText> StaticNames = TMap<FGameplayTag, FText>
	{
		{ EVSSettingItem::Audio::Volume::Master, NSLOCTEXT("VS.SettingSystem.Item.Audio.Master", "DisplayName.Master", "Master Volume") },
		{ EVSSettingItem::Audio::Volume::Music, NSLOCTEXT("VS.SettingSystem.Item.Audio.Music", "DisplayName.Music", "Music Volume") },
	};

	return StaticNames;
}

TMap<FGameplayTag, FString> UVSSettingItem_Volume::GetVolumeTypeConfigKeyNames()
{
	static TMap<FGameplayTag, FString> StaticNames = TMap<FGameplayTag, FString>
	{
		{ EVSSettingItem::Audio::Volume::Master, FString("MasterVolume") },
		{ EVSSettingItem::Audio::Volume::Music, FString("MusicVolume") },
	};

	return StaticNames;
}
#endif

