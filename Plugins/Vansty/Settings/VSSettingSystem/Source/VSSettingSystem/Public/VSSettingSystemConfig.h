// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSSettingSystemConfig.generated.h"

class UVSSettingItemAgent;

/**
 * Global developer settings for `VSSettingSystem`.
 *
 * Stores root setting-agent classes and shared display-name presets used by
 * built-in setting items when formatting option labels.
 */
UCLASS(Config = Engine, DefaultConfig)
class VSSETTINGSYSTEM_API UVSSettingSystemConfig : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the immutable default config object. */
	static const UVSSettingSystemConfig* Get();
	
	/** Blueprint accessor for the immutable default config object. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static const UVSSettingSystemConfig* GetVSSettingSystemConfig();

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	//~ End UDeveloperSettings Interface

public:
	/** Root setting-agent classes instantiated by `UVSSettingSubsystem`. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings", meta = (ConfigRestartRequired = "true"))
	TArray<TSoftClassPtr<UVSSettingItemAgent>> SettingItemAgentClasses;

	/** Localized text used to represent "System Default" options. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	FText SystemDefaultText;
	
	/** Localized text used to represent "No Limit" options. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	FText NoLimitsDisplayName;
	
	/** Display names for `false/true` states used as On/Off labels. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<bool, FText> OnOffDisplayNames;

	/** Display names for `false/true` states used as Disabled/Enabled labels. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<bool, FText> EnabledStateDisplayNames;

	/** Localized text shown when a mutable-float setting is muted. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	FText VolumeMutedText;
	
	/** Localized display names for each window mode option. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<TEnumAsByte<EWindowMode::Type>, FText> WindowModeDisplayNames;

	/** Localized display names for each anti-aliasing method option. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<TEnumAsByte<EAntiAliasingMethod>, FText> AntiAliasingMethodDisplayNames;

	/** Localized display names for motion blur quality levels, keyed by quality value. */
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<int32, FText> MotionBlueQualityLevelNames;
	
private:
#if WITH_EDITOR
	TArray<TSoftClassPtr<UVSSettingItemAgent>> EditorSettingItemAgentClasses;
#endif
};
