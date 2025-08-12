// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSChrMovOrientationTypes.h"
#include "Kismet/KismetMathLibrary.h"

bool FVSTurnInPlaceSettings2D::IsValid() const
{
	if (!UKismetMathLibrary::InRange_FloatFloat(AngleThreshold, 0.0, 180.0)) return false;	
	if (TriggerDelayTimeRange.X < 0.f || TriggerDelayTimeRange.X > TriggerDelayTimeRange.Y) return false;
	// if (!UKismetMathLibrary::InRange_FloatFloat(PlayRateMultiplierByAngle.X, -180.f, 180.f)) return false;
	// if (!UKismetMathLibrary::InRange_FloatFloat(PlayRateMultiplierByAngle.Y, -180.f, 180.f)) return false;
	// if (PlayRateMultiplierByAngle.Z < 0.f || PlayRateMultiplierByAngle.Z > PlayRateMultiplierByAngle.W) return false;

	for (const auto& AngledAnim : AngledAnims)
	{
		if (!UKismetMathLibrary::InRange_FloatFloat(AngledAnim.Key.X, -180.f, 180.f)) return false;
		if (!UKismetMathLibrary::InRange_FloatFloat(AngledAnim.Key.Y, -180.f, 180.f)) return false;
		if (TriggerDelayTimeRange.X > TriggerDelayTimeRange.Y) return false;
	}

	return true;
}
