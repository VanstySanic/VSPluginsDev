// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingSystemConfig.generated.h"

/**
 * 
 */
UCLASS(Config = "Engine", DefaultConfig, DisplayName = "VS Setting System")
class VSSETTINGSYSTEM_API UVSSettingSystemConfig : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static const UVSSettingSystemConfig* Get() { return GetDefault<UVSSettingSystemConfig>(); }
	static UVSSettingSystemConfig* GetMutable() { return const_cast<UVSSettingSystemConfig*>(Get()); }

	UPROPERTY(EditAnywhere, Config, Category = "Settings", meta = (MetaClass = "/Script/VSSettingSystem.VSSettingItemSet", ConfigRestartRequired = "true"))
	TArray<FSoftClassPath> SettingItemSetClasses;
};
