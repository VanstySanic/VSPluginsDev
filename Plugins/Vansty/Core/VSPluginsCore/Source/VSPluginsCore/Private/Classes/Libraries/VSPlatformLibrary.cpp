// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSPlatformLibrary.h"
#include "AudioDevice.h"
#include "AudioMixerBlueprintLibrary.h"
#include "AudioMixerDevice.h"
#include "Classes/Settings/VSPluginsCoreEngineSettings.h"
#include "Types/Math/VSMath.h"
#include "VSPluginsCoreCpp/Public/VSPrivablic.h"

typedef TMap<USoundMix*, TMap<USoundClass*, FSoundMixClassOverride>> FVSSoundMixClassOverrideMap;
VS_DECLARE_PRIVABLIC_MEMBER(FAudioDevice, SoundMixClassEffectOverrides, FVSSoundMixClassOverrideMap);

UVSPlatformLibrary::UVSPlatformLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool UVSPlatformLibrary::ParseSetRes(const FString& InSetRes, FIntPoint& OutResolution, TEnumAsByte<EWindowMode::Type>& OutWindowMode)
{
	FString Cmd = InSetRes;
	Cmd.TrimStartAndEndInline();

	Cmd.RemoveFromStart(TEXT("r.SetRes"));
	Cmd.TrimStartAndEndInline();

	OutWindowMode = EWindowMode::Windowed;
	if (Cmd.EndsWith(TEXT("wf")) || Cmd.EndsWith(TEXT("fw")))
	{
		OutWindowMode = EWindowMode::WindowedFullscreen;
		Cmd.LeftChopInline(2);
	}
	else if (Cmd.EndsWith(TEXT("f")))
	{
		OutWindowMode = EWindowMode::Fullscreen;
		Cmd.LeftChopInline(1);
	}
	else if (Cmd.EndsWith(TEXT("w")))
	{
		OutWindowMode = EWindowMode::Windowed;
		Cmd.LeftChopInline(1);
	}

	int32 X = 0, Y = 0;
	if (FParse::Value(*Cmd, TEXT("x"), X))
	{
		const int32 XIndex = Cmd.Find(TEXT("x"));
		const FString YStr = Cmd.Mid(XIndex + 1);
		Y = FCString::Atoi(*YStr);
	}
	else
	{
		return false;
	}

	if (X <= 0 || Y <= 0)
	{
		return false;
	}

	OutResolution = FIntPoint(X, Y);
	return true;
}

FVector2D UVSPlatformLibrary::GetWindowSize(bool bClientOnly)
{
	if (TSharedPtr<SWindow> Window = GetActiveWindow())
	{
		return bClientOnly ? Window->GetClientSizeInScreen() : Window->GetSizeInScreen();
	}

	return FVector2D::ZeroVector;
}

FVector2D UVSPlatformLibrary::GetWindowRootPosition(bool bClientOnly)
{
	TSharedPtr<SWindow> Window = GetActiveWindow();
	if (!Window) return FVector2D::ZeroVector;
	
	if (bClientOnly)
	{
		const FVector2D SizeDelta = GetWindowSize(false) - GetWindowSize(true);
		return Window->GetPositionInScreen() + SizeDelta;
	}
		
	return Window->GetPositionInScreen();
}

FVector2D UVSPlatformLibrary::GetWindowCenterPosition(bool bClientOnly)
{
	TSharedPtr<SWindow> Window = GetActiveWindow();
	if (!Window) return FVector2D::ZeroVector;
	
	const FVector2D& RootPosition = GetWindowRootPosition(bClientOnly);
	const FVector2D& WindowSize = GetWindowSize(bClientOnly);

	return RootPosition + 0.5f * WindowSize;
}

void UVSPlatformLibrary::SetWindowPosition(const FVector2D& Position)
{
	if (TSharedPtr<SWindow> Window = GetActiveWindow())
	{
		if (Window->GetWindowMode() ==  EWindowMode::Windowed)
		{
			Window->MoveWindowTo(Position);
		}
	}
}

void UVSPlatformLibrary::SetWindowCentered(bool bWorkAreaOnly)
{
	if (TSharedPtr<SWindow> Window = GetActiveWindow())
	{
		const FVector2D& CenterPos = GetWindowCenterPosition(false);
		const FString& MonitorID = GetMonitorIDByPosition(CenterPos);
		SetWindowCenteredAtMonitor(MonitorID, bWorkAreaOnly);
	}
}

void UVSPlatformLibrary::SetWindowCenteredAtMonitor(const FString& MonitorID, bool bWorkAreaOnly)
{
	if (TSharedPtr<SWindow> Window = GetActiveWindow())
	{
		const FVSMonitorInfo& MonitorInfo = GetMonitorInfoByID(MonitorID);
		if (!MonitorInfo.ID.IsEmpty())
		{
			const FVector2D& MonitorCenter = MonitorInfo.GetCenterPosition(bWorkAreaOnly);
			const FVector2D& TargetPos = MonitorCenter - Window->GetSizeInScreen() / 2.f;
			SetWindowPosition(TargetPos);
		}
	}
}


TArray<FVSMonitorInfo> UVSPlatformLibrary::GetAvailableMonitorInfos()
{
	TArray<FString> OutNames;
	TArray<FVSMonitorInfo> MonitorInfos;
	TArray<FMonitorInfo> NativeMonitorInfos = GetAvailableNativeMonitorInfos();

	for (const FMonitorInfo& NativeMonitorInfo : NativeMonitorInfos)
	{
		MonitorInfos.Emplace(NativeMonitorInfo);
	}

	return MonitorInfos;
}

FString UVSPlatformLibrary::GetPrimaryMonitorID()
{
	return GetPrimaryNativeMonitorInfo().ID;
}

bool UVSPlatformLibrary::IsValidMonitorID(const FString& MonitorID)
{
	for (const FMonitorInfo& MonitorInfo : GetAvailableNativeMonitorInfos())
	{
		if (MonitorInfo.ID == MonitorID)
		{
			return true;
		}
	}

	return false;
}

FVSMonitorInfo UVSPlatformLibrary::GetMonitorInfoByID(const FString& MonitorID)
{
	return FVSMonitorInfo(GetNativeMonitorInfoByID(MonitorID));
}

FString UVSPlatformLibrary::GetMonitorIDByPosition(const FVector2D& Position)
{
	for (const FMonitorInfo& NativeMonitorInfo : GetAvailableNativeMonitorInfos())
	{
		if (FVSMath::IsInRange(Position.X, NativeMonitorInfo.DisplayRect.Left, NativeMonitorInfo.DisplayRect.Right, true, false)
			&& FVSMath::IsInRange(Position.Y, NativeMonitorInfo.DisplayRect.Top, NativeMonitorInfo.DisplayRect.Bottom, true, false))
		{
			return NativeMonitorInfo.ID;
		}
	}

	return FString();
}

FString UVSPlatformLibrary::GetWindowRootMonitorID()
{
	TSharedPtr<SWindow> Window = GetActiveWindow();
	if (!Window.IsValid()) return FString();
	return GetMonitorIDByPosition(Window->GetPositionInScreen());
}

FVector2D UVSPlatformLibrary::GetMonitorRootPosition(const FString& MonitorID)
{
	const FMonitorInfo& MonitorInfo = GetNativeMonitorInfoByID(MonitorID);
	if (MonitorInfo.ID.IsEmpty()) return FVector2D::ZeroVector;
	return FVector2D(MonitorInfo.DisplayRect.Left, MonitorInfo.DisplayRect.Top);
}

FVector2D UVSPlatformLibrary::GetMonitorCenterPosition(const FString& MonitorID, bool bWorkAreaOnly)
{
	const FMonitorInfo& MonitorInfo = GetNativeMonitorInfoByID(MonitorID);
	if (MonitorInfo.ID.IsEmpty()) return FVector2D::ZeroVector;
	const FPlatformRect& TargetRect = bWorkAreaOnly ? MonitorInfo.WorkArea : MonitorInfo.DisplayRect;
	return 0.5f * FVector2D(TargetRect.Left + TargetRect.Right, TargetRect.Top + TargetRect.Bottom);
}

FVector2D UVSPlatformLibrary::GetMonitorCurrentResolution(const FString& MonitorID, bool bWorkAreaOnly)
{
	const FMonitorInfo& MonitorInfo = GetNativeMonitorInfoByID(MonitorID);
	if (MonitorInfo.ID.IsEmpty()) return FVector2D::ZeroVector;
	const FPlatformRect& TargetRect = bWorkAreaOnly ? MonitorInfo.WorkArea : MonitorInfo.DisplayRect;
	return FVector2D(TargetRect.Right - TargetRect.Left + 1.f, TargetRect.Bottom - TargetRect.Top + 1.f);
}

FVector2D UVSPlatformLibrary::GetMonitorMaxWindowedClientSize(const FString& MonitorID)
{
	TSharedPtr<SWindow> Window = GetActiveWindow();
	if (!Window) return FIntPoint::ZeroValue;

	const FVector2D& WindowCenterPos = GetWindowCenterPosition(true);
	const FString& MonitorIDToUse = MonitorID.IsEmpty() ? MonitorID : GetMonitorIDByPosition(WindowCenterPos);
	const FVSMonitorInfo& MonitorInfo = GetMonitorInfoByID(MonitorIDToUse);

	const FVector2D& MonitorSize = MonitorInfo.GetCurrentSize(true);
	const FMargin& ClientPaddings = Window->GetWindowBorderSize(false);
	const FVector2D& ClientSize = MonitorSize - FVector2D
	(
		ClientPaddings.Left + ClientPaddings.Right,
		ClientPaddings.Bottom + ClientPaddings.Top + Window->GetTitleBarSize().Get()
	);
	
	return ClientSize;
}

bool UVSPlatformLibrary::SwitchMonitorByID(const FString& MonitorID)
{
	TSharedPtr<SWindow> Window = GetActiveWindow();
	if (!Window.IsValid()) return false;

	FVSMonitorInfo MonitorInfo;
	if (MonitorID.IsEmpty())
	{
		MonitorInfo = GetMonitorInfoByID(GetPrimaryMonitorID());
	}
	else if (IsValidMonitorID(MonitorID))
	{
		MonitorInfo = GetMonitorInfoByID(MonitorID);
	}
	else
	{
		return false;
	}

	const EWindowMode::Type WindowMode = Window->GetWindowMode();
	
	if (WindowMode == EWindowMode::Fullscreen || WindowMode == EWindowMode::WindowedFullscreen)
	{
		Window->MoveWindowTo(MonitorInfo.GetRootPosition());
	}
	else
	{
		FVector2D NewWindowPos = MonitorInfo.GetCenterPosition(true) - Window->GetSizeInScreen() / 2.f;
		const FVector2D& WindowSize = Window->GetSizeInScreen();
		if (WindowSize.Y > MonitorInfo.GetCurrentSize(true).Y)
		{
			NewWindowPos.Y = 0.f;
		}
		Window->MoveWindowTo(NewWindowPos);
	}
	
	return true;
}

void UVSPlatformLibrary::SetDesiredFullscreenMonitorID(const FString& MonitorID)
{
	CVarDesiredFullscreenMonitorID->SetWithCurrentPriority(*MonitorID);
}

FString UVSPlatformLibrary::GetSystemDefaultAudioOutputDeviceID()
{
	for (const FAudioOutputDeviceInfo& AudioOutputDeviceInfo : GetAvailableAudioOutputDeviceInfos())
	{
		if (AudioOutputDeviceInfo.bIsSystemDefault) return AudioOutputDeviceInfo.DeviceId;
	}

	return FString();
}


FString UVSPlatformLibrary::GetActiveAudioOutputDeviceID()
{
	if (!FAudioDeviceManager::Get() || !FAudioDeviceManager::Get()->GetActiveAudioDevice()) return FString();

	FAudioDevice* ActiveAudioDevice = FAudioDeviceManager::Get()->GetActiveAudioDevice().GetAudioDevice();
	if (Audio::FMixerDevice* AudioMixerDevice = static_cast<Audio::FMixerDevice*>(ActiveAudioDevice))
	{
		if (Audio::IAudioMixerPlatformInterface* MixerPlatform = AudioMixerDevice->GetAudioMixerPlatform())
		{
			return MixerPlatform->GetPlatformDeviceInfo().DeviceId;
		}
	}

	return FString();
}

FAudioOutputDeviceInfo UVSPlatformLibrary::GetAudioOutputDeviceInfoByID(const FString& DeviceID)
{
	for (const FAudioOutputDeviceInfo& AudioOutputDeviceInfo : GetAvailableAudioOutputDeviceInfos())
	{
		if (AudioOutputDeviceInfo.DeviceId  == DeviceID) return AudioOutputDeviceInfo;
	}

	return FAudioOutputDeviceInfo();
}

TArray<FAudioOutputDeviceInfo> UVSPlatformLibrary::GetAvailableAudioOutputDeviceInfos()
{
	if (!FAudioDeviceManager::Get() || !FAudioDeviceManager::Get()->GetActiveAudioDevice()) return TArray<FAudioOutputDeviceInfo>();
	
	TArray<FAudioOutputDeviceInfo> OutputDeviceInfos;

	FAudioDevice* ActiveAudioDevice = FAudioDeviceManager::Get()->GetActiveAudioDevice().GetAudioDevice();
	if (Audio::FMixerDevice* AudioMixerDevice = static_cast<Audio::FMixerDevice*>(ActiveAudioDevice))
	{
		if (Audio::IAudioMixerPlatformInterface* MixerPlatform = AudioMixerDevice->GetAudioMixerPlatform())
		{
			if (Audio::IAudioPlatformDeviceInfoCache* DeviceInfoCache = MixerPlatform->GetDeviceInfoCache())
			{
				TArray<Audio::FAudioPlatformDeviceInfo> AllDevices = DeviceInfoCache->GetAllActiveOutputDevices();
				Algo::Transform(AllDevices, OutputDeviceInfos, [] (auto& i) -> FAudioOutputDeviceInfo { return { i }; });
			}
			else 
			{
				uint32 NumOutputDevices = 0;
				MixerPlatform->GetNumOutputDevices(NumOutputDevices);
				OutputDeviceInfos.Reserve(NumOutputDevices);
				FAudioOutputDeviceInfo CurrentOutputDevice = MixerPlatform->GetPlatformDeviceInfo();

				for (uint32 i = 0; i < NumOutputDevices; ++i)
				{
					Audio::FAudioPlatformDeviceInfo DeviceInfo;
					MixerPlatform->GetOutputDeviceInfo(i, DeviceInfo);

					FAudioOutputDeviceInfo NewInfo(DeviceInfo);
					NewInfo.bIsCurrentDevice = (NewInfo.DeviceId == CurrentOutputDevice.DeviceId);

					OutputDeviceInfos.Emplace(MoveTemp(NewInfo));
				}
			}
		}
	}

	return OutputDeviceInfos;
}

bool UVSPlatformLibrary::IsValidAudioOutputDeviceID(const FString& DeviceID)
{
	for (const FAudioOutputDeviceInfo& AudioOutputDeviceInfo : GetAvailableAudioOutputDeviceInfos())
	{
		if (AudioOutputDeviceInfo.DeviceId == DeviceID)
		{
			return true;
		}
	}

	return false;
}

bool UVSPlatformLibrary::SetActiveAudioOutputDeviceByID(const FString& DeviceID)
{
	if (!FAudioDeviceManager::Get() || !FAudioDeviceManager::Get()->GetActiveAudioDevice()) return false;

	FAudioDeviceHandle ActiveDeviceHandle = FAudioDeviceManager::Get()->GetActiveAudioDevice();
	if (!ActiveDeviceHandle.IsValid()) return false;
	
	FAudioDevice* ActiveAudioDevice = ActiveDeviceHandle.GetAudioDevice();
	if (!ActiveAudioDevice) return false;

	Audio::FMixerDevice* MixerDevice = static_cast<Audio::FMixerDevice*>(ActiveAudioDevice);
	if (!MixerDevice) return false;

	Audio::IAudioMixerPlatformInterface* MixerPlatform = MixerDevice->GetAudioMixerPlatform();
	if (!MixerPlatform) return false;
	
	const bool bSwapQueued = MixerPlatform->RequestDeviceSwap(
		DeviceID,
		false,
		TEXT("UVSPlatformLibrary::SetActiveAudioOutputDeviceByID")
	);

	return bSwapQueued;
}

FSoundClassAdjuster UVSPlatformLibrary::GetActiveSoundClassAdjuster(USoundMix* SoundMix, USoundClass* SoundClass)
{
	if (!SoundMix || !SoundClass || !FAudioDeviceManager::Get()) return FSoundClassAdjuster();

	if (FAudioDeviceHandle AudioDevice = FAudioDeviceManager::Get()->GetActiveAudioDevice())
	{
		auto& SoundMixClassOverrideMap = VS_PRIVABLIC_MEMBER(AudioDevice.GetAudioDevice(), FAudioDevice, SoundMixClassEffectOverrides);

		if (SoundMixClassOverrideMap.Contains(SoundMix) && !SoundMixClassOverrideMap[SoundMix].Contains(SoundClass))
		{
			const FSoundMixClassOverride& SoundMixClassOverride = SoundMixClassOverrideMap[SoundMix][SoundClass];
			return SoundMixClassOverride.SoundClassAdjustor;
		}

		if (AudioDevice->GetSoundMixModifiers().Contains(SoundMix))
		{
			for (const FSoundClassAdjuster& SoundClassEffect : SoundMix->SoundClassEffects)
			{
				if (SoundClassEffect.SoundClassObject == SoundClass)
				{
					return SoundClassEffect;
				}
			}
		}
	}

	return FSoundClassAdjuster();
}



TSharedPtr<SWindow> UVSPlatformLibrary::GetActiveWindow()
{
	TSharedPtr<SWindow> Window = nullptr;

	if (GEngine && GEngine->GameViewport)
	{
		Window = GEngine->GameViewport->GetWindow();
	}
	if (!Window.IsValid())
	{
		if (FSlateApplication::IsInitialized())
		{
			Window = FSlateApplication::Get().GetActiveModalWindow();
		}
	}

	return Window;
}

TArray<FMonitorInfo> UVSPlatformLibrary::GetAvailableNativeMonitorInfos()
{
	if (!FSlateApplication::IsInitialized()) return TArray<FMonitorInfo>();

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	return DisplayMetrics.MonitorInfo;
}

FMonitorInfo UVSPlatformLibrary::GetPrimaryNativeMonitorInfo()
{
	for (const FMonitorInfo& NativeMonitorInfo : GetAvailableNativeMonitorInfos())
	{
		if (NativeMonitorInfo.bIsPrimary)
		{
			return NativeMonitorInfo;
		}
	}

	return FMonitorInfo();
}

FMonitorInfo UVSPlatformLibrary::GetNativeMonitorInfoByID(const FString& MonitorID)
{
	for (const FMonitorInfo& NativeMonitorInfo : GetAvailableNativeMonitorInfos())
	{
		if (MonitorID == NativeMonitorInfo.ID)
		{
			return NativeMonitorInfo;
		}
	}

	return FMonitorInfo();
}
