// Copyright VanstySanic. All Rights Reserved.

#include "Classes/VSKeyIconConfig.h"

UTexture2D* UVSKeyIconConfig::GetIconByKey(const FKey& Key)
{
	if (UTexture2D* Icon = GetKeyIconSelf(Key)) return Icon;
	
	TArray<UVSKeyIconConfig*> Configs;
	GetAllKeyIconConfigsRecursive(Configs);
	for (UVSKeyIconConfig* Config : Configs)
	{
		if (Config)
		{
			if (UTexture2D* Icon = Config->GetKeyIconSelf(Key))
			{
				return Icon;
			}
		}
	}
	
	return nullptr;
}

void UVSKeyIconConfig::GetAllKeyIconConfigsRecursive(TArray<UVSKeyIconConfig*>& OutConfigs) const
{
	UVSKeyIconConfig* MutableThis = const_cast<UVSKeyIconConfig*>(this);
	if (OutConfigs.Contains(MutableThis)) return;
	OutConfigs.AddUnique(MutableThis);
	
	TArray<UVSKeyIconConfig*> ConfigsToAppend;
	for (UVSKeyIconConfig* KeyIconConfig : KeyIconConfigs)
	{
		if (KeyIconConfig && !OutConfigs.Contains(KeyIconConfig))
		{
			KeyIconConfig->GetAllKeyIconConfigsRecursive(OutConfigs);
		}
	}
}

UTexture2D* UVSKeyIconConfig::GetKeyIconSelf(const FKey& Key) const
{
	if (KeyedIcons.Contains(Key) && KeyedIcons[Key])
	{
		return KeyedIcons[Key];
	}

	return nullptr;
}
