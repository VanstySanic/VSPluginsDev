// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Classes/Settings/VSPluginsCoreEngineSettings.h"
#include "Types/Math/VSMath.h"

UVSPlatformLibrary::UVSPlatformLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
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

	return 0.5f * (RootPosition + WindowSize);
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


TArray<FVSMonitorInfo> UVSPlatformLibrary::GetAllMonitorInfos()
{
	TArray<FString> OutNames;
	TArray<FVSMonitorInfo> MonitorInfos;
	TArray<FMonitorInfo> NativeMonitorInfos = GetAllNativeMonitorInfos();

	for (const FMonitorInfo& NativeMonitorInfo : NativeMonitorInfos)
	{
		MonitorInfos.Emplace(NativeMonitorInfo);
	}

	return MonitorInfos;
}

FVSMonitorInfo UVSPlatformLibrary::GetPrimaryMonitorInfo()
{
	return FVSMonitorInfo(GetPrimaryNativeMonitorInfo());
}

FVSMonitorInfo UVSPlatformLibrary::GetMonitorInfoByID(const FString& MonitorID)
{
	return FVSMonitorInfo(GetNativeMonitorInfoByID(MonitorID));
}

FString UVSPlatformLibrary::GetMonitorIDByPosition(const FVector2D& Position)
{
	for (const FMonitorInfo& NativeMonitorInfo : GetAllNativeMonitorInfos())
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

	const FVector2D& MonitorSize = MonitorInfo.GetCurrentResolution(true);
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
	
	const FVSMonitorInfo MonitorInfo = GetMonitorInfoByID(MonitorID);
	if (MonitorInfo.ID.IsEmpty()) return false;

	const EWindowMode::Type WindowMode = Window->GetWindowMode();
	
	if (WindowMode == EWindowMode::Fullscreen || WindowMode == EWindowMode::WindowedFullscreen)
	{
		Window->MoveWindowTo(MonitorInfo.GetRootPosition());
	}
	else
	{
		FVector2D NewWindowPos = MonitorInfo.GetCenterPosition(true) - Window->GetSizeInScreen() / 2.f;
		const FVector2D& WindowSize = Window->GetSizeInScreen();
		if (WindowSize.Y > MonitorInfo.GetCurrentResolution(true).Y)
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

TArray<FMonitorInfo> UVSPlatformLibrary::GetAllNativeMonitorInfos()
{
	if (!FSlateApplication::IsInitialized()) return TArray<FMonitorInfo>();

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	return DisplayMetrics.MonitorInfo;
}

FMonitorInfo UVSPlatformLibrary::GetPrimaryNativeMonitorInfo()
{
	for (const FMonitorInfo& NativeMonitorInfo : GetAllNativeMonitorInfos())
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
	for (const FMonitorInfo& NativeMonitorInfo : GetAllNativeMonitorInfos())
	{
		if (MonitorID == NativeMonitorInfo.ID)
		{
			return NativeMonitorInfo;
		}
	}

	return FMonitorInfo();
}
