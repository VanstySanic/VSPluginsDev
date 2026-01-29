// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundMix.h"
#include "Types/VSPlatformTypes.h"
#include "VSPlatformLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API
UVSPlatformLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "Platform|Utils", meta = (AutoCreateRefTerm = "InCmd"))
	static bool ParseSetRes(const FString& InSetRes, FIntPoint& OutResolution, TEnumAsByte<EWindowMode::Type>& OutWindowMode);

	
	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetWindowSize(bool bClientOnly = false);

	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetWindowRootPosition(bool bClientOnly = false);

	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetWindowCenterPosition(bool bClientOnly = false);

	UFUNCTION(BlueprintCallable, Category = "Platform|Window", meta = (AutoCreateRefTerm = "Position"))
	static void SetWindowPosition(const FVector2D& Position);

	UFUNCTION(BlueprintCallable, Category = "Platform|Window")
	static void SetWindowCentered(bool bWorkAreaOnly = true);

	UFUNCTION(BlueprintCallable, Category = "Platform|Window", meta = (AutoCreateRefTerm = "MonitorID"))
	static void SetWindowCenteredAtMonitor(const FString& MonitorID, bool bWorkAreaOnly = true);

	
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static TArray<FVSMonitorInfo> GetAvailableMonitorInfos();

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static FString GetPrimaryMonitorID();

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static bool IsValidMonitorID(const FString& MonitorID);
	
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVSMonitorInfo GetMonitorInfoByID(const FString& MonitorID);

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "Position"))
	static FString GetMonitorIDByPosition(const FVector2D& Position);

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static FString GetWindowRootMonitorID();

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorRootPosition(const FString& MonitorID);

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorCenterPosition(const FString& MonitorID, bool bWorkAreaOnly = false);

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorCurrentResolution(const FString& MonitorID, bool bWorkAreaOnly = false);

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorMaxWindowedClientSize(const FString& MonitorID);

	/**
	 * @params MonitorID The monitor id to set. If left empty, use the default one.
	 */
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static bool SwitchMonitorByID(const FString& MonitorID);

	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static void SetDesiredFullscreenMonitorID(const FString& MonitorID);


	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FString GetSystemDefaultAudioOutputDeviceID();
	
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FString GetActiveAudioOutputDeviceID();
	
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "DeviceID"))
	static FAudioOutputDeviceInfo GetAudioOutputDeviceInfoByID(const FString& DeviceID);
	
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static TArray<FAudioOutputDeviceInfo> GetAvailableAudioOutputDeviceInfos();

	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "DeviceID"))
	static bool IsValidAudioOutputDeviceID(const FString& DeviceID);

	/**
	 * @params DeviceID The device id to set. If left empty, use the primary one.
	 */
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "DeviceID"))
	static bool SetActiveAudioOutputDeviceByID(const FString& DeviceID);
	
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FSoundClassAdjuster GetActiveSoundClassAdjuster(USoundMix* SoundMix, USoundClass* SoundClass);

	
public:
	static TSharedPtr<SWindow> GetActiveWindow();
	static TArray<FMonitorInfo> GetAvailableNativeMonitorInfos();
	
	static FMonitorInfo GetPrimaryNativeMonitorInfo();
	static FMonitorInfo GetNativeMonitorInfoByID(const FString& MonitorID);


};
