// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSChrMovWallRunTypes.h"
#include "Kismet/KismetMathLibrary.h"

bool FVSWallRunLimits::IsValid() const
{
	if (RootHeightToGroundThresholdRange.X <= 0.f || RootHeightToGroundThresholdRange.X > RootHeightToGroundThresholdRange.Y) return false;
	if (FacingMovementAngleRange2D.X > FacingMovementAngleRange2D.Y) return false;
	if (VelocityTowardsMovementAngleRange2D.X > VelocityTowardsMovementAngleRange2D.Y) return false;
	if (InputTowardsMovementAngleRange2D.X > InputTowardsMovementAngleRange2D.Y) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(FacingMovementAngleRange2D.X, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(FacingMovementAngleRange2D.Y, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityTowardsMovementAngleRange2D.X, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityTowardsMovementAngleRange2D.Y, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(InputTowardsMovementAngleRange2D.X, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(InputTowardsMovementAngleRange2D.Y, -180.f, 180.f)) return false;

	
	return true;
}

bool FVSWallRunSettings::IsValid() const
{
	if (!Limits.IsValid()) return false;
	if (!LeftAnims.CycleAnim) return false;
	if (!RightAnims.CycleAnim) return false;
	if (CycleDistanceToWall <= 0.f) return false;
	if (CycleSpeed <= 0.f) return false;
	
	return true;
}
