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
class VSPLUGINSCORE_API UVSPlatformLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Parse a r.SetRes style string into resolution and window mode. */
	UFUNCTION(BlueprintPure, Category = "Platform|Utils", meta = (AutoCreateRefTerm = "InCmd"))
	static bool ParseSetRes(const FString& InSetRes, FIntPoint& OutResolution, TEnumAsByte<EWindowMode::Type>& OutWindowMode);

	
	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetGameWindowSize(bool bClientOnly = false);

	/** Get the game window's top-left position in screen space. */
	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetGameWindowRootPosition(bool bClientOnly = false);

	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetGameWindowCenterPosition(bool bClientOnly = false);

	/** Move the game window root to the given screen position (windowed only). */
	UFUNCTION(BlueprintCallable, Category = "Platform|Window", meta = (AutoCreateRefTerm = "Position"))
	static void SetGameWindowPosition(const FVector2D& Position);

	/** Center the game window on its current monitor. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Window")
	static void SetGameWindowCentered(bool bWorkAreaOnly = true);

	/** Center the game window on the specified monitor. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Window", meta = (AutoCreateRefTerm = "MonitorID"))
	static void SetGameWindowCenteredAtMonitor(const FString& MonitorID, bool bWorkAreaOnly = true);

	
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static FString GetPrimaryMonitorID();

	/** Get monitor info for all available displays. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static TArray<FVSMonitorInfo> GetAvailableMonitorInfos();

	/** Check if a monitor ID exists on the system. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static bool IsValidMonitorID(const FString& MonitorID);
	
	/** Get a monitor info struct by ID (empty if not found). */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVSMonitorInfo GetMonitorInfoByID(const FString& MonitorID);

	/** Get the monitor ID containing a screen position. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "Position"))
	static FString GetMonitorIDByPosition(const FVector2D& Position);

	/** Get the monitor ID that contains the game window's root position. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static FString GetWindowRootMonitorID();

	/** Get the top-left position of a monitor. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorRootPosition(const FString& MonitorID);

	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorCenterPosition(const FString& MonitorID, bool bWorkAreaOnly = false);

	/** Get the monitor resolution in pixels. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorCurrentResolution(const FString& MonitorID, bool bWorkAreaOnly = false);

	/** Get the max client area size for a window on the monitor. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorMaximumWindowedClientSize(const FString& MonitorID);

	/**
	 * Move the game window to another monitor.
	 * @params MonitorID The monitor id to set. If left empty, use the default one.
	 */
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static bool SwitchMonitorByID(const FString& MonitorID);

	/** Set the preferred fullscreen monitor ID. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static void SetDesiredFullscreenMonitorID(const FString& MonitorID);

	
	/** Get the system default audio output device ID. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FString GetSystemDefaultAudioOutputDeviceID();
	
	/** Get the currently active audio output device ID. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FString GetActiveAudioOutputDeviceID();
	
	/** Get audio output device info by ID. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Audio", meta = (AutoCreateRefTerm = "DeviceID"))
	static FAudioOutputDeviceInfo GetAudioOutputDeviceInfoByID(const FString& DeviceID);
	
	/** Get all available audio output device infos. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static TArray<FAudioOutputDeviceInfo> GetAvailableAudioOutputDeviceInfos();

	/** Check if an audio device ID exists on the system. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Audio", meta = (AutoCreateRefTerm = "DeviceID"))
	static bool IsValidAudioOutputDeviceID(const FString& DeviceID);

	/**
	 * Request the active audio output device to switch.
	 * @params DeviceID The device id to set. If left empty, use the primary one.
	 */
	UFUNCTION(BlueprintCallable, Category = "Platform|Audio", meta = (AutoCreateRefTerm = "DeviceID"))
	static bool SetActiveAudioOutputDeviceByID(const FString& DeviceID);
	
	/** Get the active sound class adjuster for a sound mix. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FSoundClassAdjuster GetActiveSoundClassAdjuster(USoundMix* SoundMix, USoundClass* SoundClass);

public:
	/** Get the game's slate window. */
	static TSharedPtr<SWindow> GetGameWindow();
	
	/** Get native monitor info from the platform. */
	static TArray<FMonitorInfo> GetAvailableNativeMonitorInfos();
	
	/** Get the primary native monitor info. */
	static FMonitorInfo GetPrimaryNativeMonitorInfo();
	
	/** Get native monitor info by ID. */
	static FMonitorInfo GetNativeMonitorInfoByID(const FString& MonitorID);
};
