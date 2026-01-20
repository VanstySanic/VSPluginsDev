// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSPlatformTypes.generated.h"

USTRUCT(BlueprintType)
struct FVSMonitorInfo
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
	FVector2D GetCurrentResolution(bool bWorkAreaOnly = false) const;

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString ID;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint NativeSize = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint MaxResolution = FIntPoint(ForceInitToZero);

	UPROPERTY(BlueprintReadOnly)
	FIntRect DisplayRect;
	
	UPROPERTY(BlueprintReadOnly)
	FIntRect WorkArea;

	bool bIsPrimary = false;
	int32 DPI = 0;
};
