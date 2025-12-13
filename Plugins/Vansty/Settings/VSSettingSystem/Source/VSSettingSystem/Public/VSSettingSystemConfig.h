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

	virtual FName GetCategoryName() const override;

public:
	UPROPERTY(EditAnywhere, Config, Category = "Settings", meta = (ConfigRestartRequired = "true"))
	TArray<TSoftClassPtr<UVSSettingItemAgent>> SettingItemAgentClasses;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Config, Category = "Settings")
	uint8 bIgnoreEditorSettingItemArrayNotification : 1;
#endif
	
};
