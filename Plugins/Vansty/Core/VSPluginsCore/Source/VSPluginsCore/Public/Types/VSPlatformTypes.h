// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSPlatformTypes.generated.h"

/**
 * Blueprint-friendly snapshot of a physical monitor descriptor.
 */
USTRUCT(BlueprintType)
struct VSPLUGINSCORE_API FVSMonitorInfo
{
	GENERATED_BODY()

	FVSMonitorInfo() {}
	FVSMonitorInfo(const FMonitorInfo& MonitorInfo)
		: Name(MonitorInfo.Name)
		, ID(MonitorInfo.ID)
		, MaxResolution(MonitorInfo.MaxResolution)
		, DPI(MonitorInfo.DPI)
	{
		NativeSize.X = MonitorInfo.NativeWidth;
		NativeSize.Y = MonitorInfo.NativeHeight;
		DisplayRect = FIntRect(MonitorInfo.DisplayRect.Left, MonitorInfo.DisplayRect.Top, MonitorInfo.DisplayRect.Right, MonitorInfo.DisplayRect.Bottom);
		WorkArea = FIntRect(MonitorInfo.WorkArea.Left, MonitorInfo.WorkArea.Top, MonitorInfo.WorkArea.Right, MonitorInfo.WorkArea.Bottom);
	}

	FVector2D GetRootPosition() const;
	FVector2D GetCenterPosition(bool bWorkAreaOnly = false) const;
	FVector2D GetCurrentSize(bool bWorkAreaOnly = false) const;

	/** Display name reported by the platform. */
	UPROPERTY(BlueprintReadOnly)
	FString Name;

	/** Stable monitor identifier reported by the platform. */
	UPROPERTY(BlueprintReadOnly)
	FString ID;

	/** Native panel pixel size. */
	UPROPERTY(BlueprintReadOnly)
	FIntPoint NativeSize = FIntPoint::ZeroValue;

	/** Max selectable resolution reported by the platform. */
	UPROPERTY(BlueprintReadOnly)
	FIntPoint MaxResolution = FIntPoint::ZeroValue;

	/** Full monitor rect in desktop coordinates. */
	UPROPERTY(BlueprintReadOnly)
	FIntRect DisplayRect;
	
	/** Work-area rect in desktop coordinates (taskbar excluded). */
	UPROPERTY(BlueprintReadOnly)
	FIntRect WorkArea;

	/** True when this monitor is the platform primary display. */
	bool bIsPrimary = false;
	/** Display DPI reported by the platform. */
	int32 DPI = 0;
};
