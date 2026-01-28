// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSPlatformTypes.h"

FVector2D FVSMonitorInfo::GetRootPosition() const
{
	return FVector2D(DisplayRect.Min.X, DisplayRect.Min.Y);
}

FVector2D FVSMonitorInfo::GetCenterPosition(bool bWorkAreaOnly) const
{
	const FIntRect& TargetRect = bWorkAreaOnly ? WorkArea : DisplayRect;
	return 0.5f * FVector2D(TargetRect.Max.X + TargetRect.Min.X, TargetRect.Max.Y + TargetRect.Min.Y);
}

FVector2D FVSMonitorInfo::GetCurrentSize(bool bWorkAreaOnly) const
{
	const FIntRect& TargetRect = bWorkAreaOnly ? WorkArea : DisplayRect;
	return FVector2D(TargetRect.Max.X - TargetRect.Min.X, TargetRect.Max.Y - TargetRect.Min.Y);
}
