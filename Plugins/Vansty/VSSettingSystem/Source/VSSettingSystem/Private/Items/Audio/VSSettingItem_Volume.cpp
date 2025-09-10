// Copyright VanstySanic. All Rights Reserved.

#include "Items/Audio/VSSettingItem_Volume.h"
#include "AudioDevice.h"
#include "VSSettingSystemLibrary.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_Volume::UVSSettingItem_Volume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio_Volume_Master);
	
	ValueUpdatedActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::Validate,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
			EVSSettingItemAction::Save
		};

	DesiredValueRange = FVector2D(0.f, 100.f);
	DesiredValueStep = 1.f;
	DisplayFractionNum = 0;
	
	MutedText = NSLOCTEXT("VSSettingSystem", "Settings.Audio.Volume.Muted", "Muted");
}

void UVSSettingItem_Volume::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	OnAudioDeviceCreatedDelegateHandle = FAudioDeviceManagerDelegates::OnAudioDeviceCreated.AddLambda([this] (Audio::FDeviceId AudioDeviceId)
	{
		if (!FAudioDeviceManager::Get()) return;
		if (FAudioDeviceHandle AudioDevice = FAudioDeviceManager::Get()->GetAudioDevice(AudioDeviceId))
		{
			ApplyToAudioDevice(AudioDevice.GetAudioDevice());
		}
	});

	ContentTextFormat = FText::FromString("{0}%");
}

void UVSSettingItem_Volume::Uninitialize_Implementation()
{
	if (OnAudioDeviceCreatedDelegateHandle.IsValid())
	{
		FAudioDeviceManagerDelegates::OnAudioDeviceCreated.Remove(OnAudioDeviceCreatedDelegateHandle);
	}

	Super::Uninitialize_Implementation();
}

void UVSSettingItem_Volume::Load_Implementation()
{
	Super::Load_Implementation();
}

void UVSSettingItem_Volume::Apply_Implementation()
{
	if (!FAudioDeviceManager::Get()) return;
	for (FAudioDevice* AudioDevice : FAudioDeviceManager::Get()->GetAudioDevices())
	{
		if (AudioDevice)
		{
			ApplyToAudioDevice(AudioDevice);
		}	
	}
}

void UVSSettingItem_Volume::Confirm_Implementation()
{
	LastConfirmedVolume = Volume;
	bLastConfirmedForceMute = bForceMute;
}

void UVSSettingItem_Volume::Save_Implementation()
{
	
}

void UVSSettingItem_Volume::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Mute")))
	{
		if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			CheckBox->OnCheckStateChanged.RemoveDynamic(this, & UVSSettingItem_Volume::OnMuteCheckBoxCheckStateChanged);
			CheckBox->SetIsChecked(bForceMute || FMath::IsNearlyZero(Volume));
			CheckBox->OnCheckStateChanged.AddDynamic(this, & UVSSettingItem_Volume::OnMuteCheckBoxCheckStateChanged);

			CheckBox->SetIsEnabled(!FMath::IsNearlyZero(Volume));
		}
	}
	
	const FText& SettingText = (bForceMute || FMath::IsNearlyZero(Volume)) ? MutedText : GetValueContentText(Volume);
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItem_Volume::SetValue_Implementation(float InValue)
{
	SetVolume(InValue);
}

float UVSSettingItem_Volume::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetVolume(ValueSource);
}

void UVSSettingItem_Volume::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	Super::BindWidgetInternal_Implementation(Widget, TypeName);
	
	if (TypeName == FName("Mute"))
	{
		if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			CheckBox->SetIsChecked(bForceMute || FMath::IsNearlyZero(Volume));
			if (UVSGameplayLibrary::IsInGame())
			{
				CheckBox->OnCheckStateChanged.AddDynamic(this, & UVSSettingItem_Volume::OnMuteCheckBoxCheckStateChanged);
			}
		}
	}
	
	if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			const FText& SettingText = (bForceMute || FMath::IsNearlyZero(Volume)) ? MutedText : GetValueContentText(Volume);
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItem_Volume::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Mute"))
	{
		if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				CheckBox->OnCheckStateChanged.RemoveDynamic(this, & UVSSettingItem_Volume::OnMuteCheckBoxCheckStateChanged);
			}
		}
	}
	
	Super::UnbindWidgetInternal_Implementation(Widget, TypeName);
}

void UVSSettingItem_Volume::SetVolume(float InVolume)
{
	if (FMath::IsNearlyEqual(Volume, InVolume)) return;
	int a = Volume;
	int b = InVolume;
	Volume = InVolume;
	NotifyValueUpdate();
	
	SetForceMute(false);
}

float UVSSettingItem_Volume::GetVolume(EVSSettingItemValueSource::Type ValueSource) const
{
	if (!SoundMix || !SoundClass) return 0.f;
	
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		{
			for (const FSoundClassAdjuster& SoundClassEffect : SoundMix->SoundClassEffects)
			{
				if (SoundClassEffect.SoundClassObject == SoundClass)
				{
					return SoundClassEffect.VolumeAdjuster;
				}
			}
		}
		return 0.f;
	
	case EVSSettingItemValueSource::Current:
		return 100.f * UVSSettingSystemLibrary::GetActiveSoundClassAdjuster(SoundMix, SoundClass).VolumeAdjuster;
			
	case EVSSettingItemValueSource::Settings:
		return Volume;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedVolume;
		
	default:
		return 0.f;
	}
}

void UVSSettingItem_Volume::SetForceMute(bool bInForceMute)
{
	if (bForceMute == bInForceMute) return;
	bForceMute = bInForceMute;
	NotifyValueUpdate();
}

bool UVSSettingItem_Volume::GetForceMute(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return false;
			
	case EVSSettingItemValueSource::Current:
		return bAppliedForceMute;
			
	case EVSSettingItemValueSource::Settings:
		return bForceMute;

	case EVSSettingItemValueSource::LastConfirmed:
		return bLastConfirmedForceMute;
		
	default:
		return false;
	}
}

void UVSSettingItem_Volume::ApplyToAudioDevice(FAudioDevice* AudioDevice)
{
	if (!AudioDevice || !SoundMix || !SoundClass) return;
	
	bAppliedForceMute = bForceMute;
	const FSoundClassAdjuster& SoundClassAdjuster = UVSSettingSystemLibrary::GetActiveSoundClassAdjuster(SoundMix, SoundClass);
	AudioDevice->PushSoundMixModifier(SoundMix);
	AudioDevice->SetSoundMixClassOverride(SoundMix, SoundClass, bAppliedForceMute ? 0.f : (Volume / 100.f), SoundClassAdjuster.PitchAdjuster, SoundMix->FadeInTime, SoundClassAdjuster.bApplyToChildren);
}

void UVSSettingItem_Volume::OnMuteCheckBoxCheckStateChanged(bool bIsChecked)
{
	// if (!FMath::IsNearlyZero(Volume))
	{
		SetForceMute(bIsChecked);
	}
}
