// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundMix.h"
#include "Types/VSPlatformTypes.h"
#include "VSPlatformLibrary.generated.h"

/**
 * Blueprint platform helpers.
 */
UCLASS()
class VSPLUGINSCORE_API UVSPlatformLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Parse r.SetRes style string into resolution and window mode. */
	UFUNCTION(BlueprintPure, Category = "Platform|Utils", meta = (AutoCreateRefTerm = "InCmd"))
	static bool ParseSetRes(const FString& InSetRes, FIntPoint& OutResolution, TEnumAsByte<EWindowMode::Type>& OutWindowMode);

	/** Returns current game-window size. If bClientOnly, excludes non-client frame/border. */
	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetGameWindowSize(bool bClientOnly = false);

	/** Returns game window top-left position in screen space. */
	UFUNCTION(BlueprintPure, Category = "Platform|Window")
	static FVector2D GetGameWindowRootPosition(bool bClientOnly = false);

	/** Returns game-window center position in screen space. */
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

	/** Returns platform primary monitor ID. Empty when unavailable. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static FString GetPrimaryMonitorID();

	/** Returns monitor info for all currently available displays. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static TArray<FVSMonitorInfo> GetAvailableMonitorInfos();

	/** Check if a monitor ID exists on the system. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static bool IsValidMonitorID(const FString& MonitorID);
	
	/** Returns monitor info by ID. If not found, returns default/empty info. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVSMonitorInfo GetMonitorInfoByID(const FString& MonitorID);

	/** Returns the monitor ID that contains the given screen-space position. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "Position"))
	static FString GetMonitorIDByPosition(const FVector2D& Position);

	/** Returns the monitor ID containing the game window root position. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor")
	static FString GetWindowRootMonitorID();

	/** Returns monitor top-left position in virtual desktop coordinates. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorRootPosition(const FString& MonitorID);

	/** Returns monitor center position in virtual desktop coordinates. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorCenterPosition(const FString& MonitorID, bool bWorkAreaOnly = false);

	/** Returns monitor resolution in pixels (full area or work area). */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorCurrentResolution(const FString& MonitorID, bool bWorkAreaOnly = false);

	/** Returns maximum windowed client size available on the monitor. */
	UFUNCTION(BlueprintPure, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static FVector2D GetMonitorMaximumWindowedClientSize(const FString& MonitorID);

	/**
	 * Move the game window to another monitor.
	 * @param MonitorID The monitor id to set. If empty, uses platform default.
	 */
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static bool SwitchMonitorByID(const FString& MonitorID);

	/** Stores preferred monitor ID used by fullscreen switching logic. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Monitor", meta = (AutoCreateRefTerm = "MonitorID"))
	static void SetDesiredFullscreenMonitorID(const FString& MonitorID);

	
	/** Returns system default audio output device ID. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FString GetSystemDefaultAudioOutputDeviceID();
	
	/** Returns currently active audio output device ID. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FString GetActiveAudioOutputDeviceID();
	
	/** Returns audio output device info by ID. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Audio", meta = (AutoCreateRefTerm = "DeviceID"))
	static FAudioOutputDeviceInfo GetAudioOutputDeviceInfoByID(const FString& DeviceID);
	
	/** Returns all currently available audio output devices. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static TArray<FAudioOutputDeviceInfo> GetAvailableAudioOutputDeviceInfos();

	/** Check if an audio device ID exists on the system. */
	UFUNCTION(BlueprintCallable, Category = "Platform|Audio", meta = (AutoCreateRefTerm = "DeviceID"))
	static bool IsValidAudioOutputDeviceID(const FString& DeviceID);

	/**
	 * Request the active audio output device to switch.
	 * @param DeviceID The target device id. If empty, uses system default.
	 */
	UFUNCTION(BlueprintCallable, Category = "Platform|Audio", meta = (AutoCreateRefTerm = "DeviceID"))
	static bool SetActiveAudioOutputDeviceByID(const FString& DeviceID);
	
	/** Returns active sound-class adjuster entry for the given mix/class pair. */
	UFUNCTION(BlueprintPure, Category = "Platform|Audio")
	static FSoundClassAdjuster GetActiveSoundClassAdjuster(USoundMix* SoundMix, USoundClass* SoundClass);

public:
	/** Returns the game Slate window if available. */
	static TSharedPtr<SWindow> GetGameWindow();
	
	/** Returns raw native monitor info from the platform layer. */
	static TArray<FMonitorInfo> GetAvailableNativeMonitorInfos();
	
	/** Returns raw native info for the primary monitor. */
	static FMonitorInfo GetPrimaryNativeMonitorInfo();
	
	/** Returns raw native monitor info by monitor ID. */
	static FMonitorInfo GetNativeMonitorInfoByID(const FString& MonitorID);
};
