// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSChrMovMantleVaultTypes.h"
#include "Kismet/KismetMathLibrary.h"

bool FVSMantleVaultAnimSettings::IsValid(const FName AnimScaleMovementToReachTargetTimeMarkName, const FName AnimReachTargetTimeMarkName, const FName AnimGroundPivotTimeMarkName, const FName AnimVaultOffPlatformTimeMarkName) const
{
	FVSAnimSequenceReference* Anim = AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!Anim || !Anim->IsValid()) return false;

	if (MovementType == EVSMantleVaultMovementType::None) return false;
	if (ReachTargetDistanceToWallRange.X > ReachTargetDistanceToWallRange.Y) return false;

	if (!Anim->HasTimeMark(AnimReachTargetTimeMarkName)) return false;
	if (!Anim->HasTimeMark(AnimGroundPivotTimeMarkName)) return false;

	if (Anim->HasTimeMark(AnimScaleMovementToReachTargetTimeMarkName) && Anim->GetMarkTime(AnimScaleMovementToReachTargetTimeMarkName) > Anim->GetMarkTime(AnimReachTargetTimeMarkName)) return false;

	const bool bSupportVault = MovementType & EVSMantleVaultMovementType::Vault;
	if (bSupportVault && !Anim->HasTimeMark(AnimVaultOffPlatformTimeMarkName)) return false;

	const float GroundPivotTime = Anim->GetMarkTime(AnimGroundPivotTimeMarkName);
	if (Anim->GetMarkTime(AnimReachTargetTimeMarkName) > GroundPivotTime) return false;
	if (bSupportVault && GroundPivotTime > Anim->GetMarkTime(AnimVaultOffPlatformTimeMarkName)) return false;
	
	return true;
}

bool FVSMantleVaultLimits::IsValid() const
{
	if (PlatformHeightRange.X < 0.f || PlatformHeightRange.X > PlatformHeightRange.Y) return false;
	if (DistanceToWallRange2D.X < 0.f || DistanceToWallRange2D.X > DistanceToWallRange2D.Y) return false;
	if (SpeedTowardsMovementRange2D.X < 0.f || SpeedTowardsMovementRange2D.X > SpeedTowardsMovementRange2D.Y) return false;
	if (FacingMovementMaxAngle < 0.f) return false;
	if (Velocity2DTowardsMovementMaxAngle < 0.f) return false;
	if (Input2DTowardsMovementMaxAngle < 0.f) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(FacingMovementMaxAngle, 0.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(Velocity2DTowardsMovementMaxAngle, 0.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(Input2DTowardsMovementMaxAngle, 0.f, 180.f)) return false;

	return true;
}

bool FVSMantleVaultSettings::IsValid() const
{
	if (!Limits.IsValid()) return false;
	if (AnimRows.IsEmpty()) return false;
	
	return true;
}
