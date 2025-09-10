// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemLibrary.h"
#include "AudioDevice.h"
#include "AudioDeviceManager.h"
#include "VSPrivablic.h"

typedef TMap<USoundMix*, TMap<USoundClass*, FSoundMixClassOverride>> FVSSoundMixClassOverrideMap;
VS_DECLARE_PRIVABLIC_MEMBER(FAudioDevice, SoundMixClassEffectOverrides, FVSSoundMixClassOverrideMap);

UVSSettingSystemLibrary::UVSSettingSystemLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FSoundClassAdjuster UVSSettingSystemLibrary::GetActiveSoundClassAdjuster(USoundMix* SoundMix, USoundClass* SoundClass)
{
	if (!SoundMix || !SoundClass || !FAudioDeviceManager::Get()) return FSoundClassAdjuster();

	if (FAudioDeviceHandle AudioDevice = FAudioDeviceManager::Get()->GetActiveAudioDevice())
	{
		FVSSoundMixClassOverrideMap& SoundMixClassOverrideMap = VS_PRIVABLIC_MEMBER(AudioDevice.GetAudioDevice(), FAudioDevice, SoundMixClassEffectOverrides);

		if (SoundMixClassOverrideMap.Contains(SoundMix) && !SoundMixClassOverrideMap[SoundMix].Contains(SoundClass))
		{
			const FSoundMixClassOverride& SoundMixClassOverride = SoundMixClassOverrideMap[SoundMix][SoundClass];
			return SoundMixClassOverride.SoundClassAdjustor;
		}
	}

	for (const FSoundClassAdjuster& SoundClassEffect : SoundMix->SoundClassEffects)
	{
		if (SoundClassEffect.SoundClassObject == SoundClass)
		{
			return SoundClassEffect;
		}
	}

	return FSoundClassAdjuster();
}

