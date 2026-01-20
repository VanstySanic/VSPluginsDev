// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSSettingSystemConfig.generated.h"

class UVSSettingItemAgent;
/**
 * 
 */
UCLASS(Config = Engine, DefaultConfig)
class VSSETTINGSYSTEM_API UVSSettingSystemConfig : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	static const UVSSettingSystemConfig* Get();
	
	UFUNCTION(BlueprintPure, Category = "Settings")
	static const UVSSettingSystemConfig* GetSettingSystemConfig_VS();

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
	UPROPERTY(EditAnywhere, Config, Category = "Settings", meta = (ConfigRestartRequired = "true"))
	TArray<TSoftClassPtr<UVSSettingItemAgent>> SettingItemAgentClasses;

	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	FText NoLimitsDisplayName;
	
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<bool, FText> OnOffDisplayNames;

	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<bool, FText> EnabledStateDisplayNames;
	
	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<TEnumAsByte<EWindowMode::Type>, FText> WindowModeDisplayNames;

	UPROPERTY(EditAnywhere, Config, Category = "Settings|Display Names", meta = (ConfigRestartRequired = "true"))
	TMap<TEnumAsByte<EAntiAliasingMethod>, FText> AntiAliasingMethodDisplayNames;
	
private:
#if WITH_EDITOR
	TArray<TSoftClassPtr<UVSSettingItemAgent>> EditorSettingItemAgentClasses;
#endif
};
