// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VSPluginsCoreEngineSettings.generated.h"

static TAutoConsoleVariable<FString> CVarDesiredFullscreenMonitorID(
	TEXT("r.DesiredFullscreenMonitorID"),
	"",
	TEXT("Target monitor ID for exclusive fullscreen (fallbacks to primary if empty or invalid)."),
	ECVF_Scalability);

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSPluginsCoreEngineSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UDeveloperSettings Interface
	virtual void PostInitProperties() override;
	virtual FName GetCategoryName() const override;
	//~ End UDeveloperSettings Interface
	
	static const UVSPluginsCoreEngineSettings* Get();
	
	UFUNCTION(BlueprintPure, Category = "Settings")
	static const UVSPluginsCoreEngineSettings* GetVSPluginsCoreEngineSettings();
	
public:


private:
	void CheckFullscreenDesiredMonitor();

private:
	void OnCVarDesiredFullscreenMonitorIDChanged(IConsoleVariable* ConsoleVariable);
	void OnApplicationActivationStateChanged(bool bIsApplicationActive);
};
