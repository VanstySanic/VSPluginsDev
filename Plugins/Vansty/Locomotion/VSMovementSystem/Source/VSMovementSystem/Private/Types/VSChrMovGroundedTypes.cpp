// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSChrMovGroundedTypes.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Types/Animation/VSAnimSequenceReference.h"

struct FVSAnimSequenceReference;

bool FVSLandRecoveryLimits::IsValid() const
{
	if (SpeedRange2D.X < 0.f || SpeedRange2D.X > SpeedRange2D.Y) return false;
	if (SpeedRangeBeforeLandingZ.X < 0.f || SpeedRangeBeforeLandingZ.X > SpeedRangeBeforeLandingZ.Y) return false;
	if (VelocityAngleRangePitch.X > VelocityAngleRangePitch.Y
		|| !UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangePitch.X, -180.f, 180.f)
		|| !UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangePitch.Y, -180.f, 180.f)) return false;
	if (VelocityAngleRangeBeforeLandingYaw.X > VelocityAngleRangeBeforeLandingYaw.Y
		|| !UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangeBeforeLandingYaw.X, -180.f, 180.f)
		|| !UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangeBeforeLandingYaw.Y, -180.f, 180.f)) return false;
	
	return true;
}

bool FVSLandRecoveryLimits::Validate()
{
	if (SpeedRange2D.X > SpeedRange2D.Y) Swap(SpeedRange2D.X, SpeedRange2D.Y);
	if (SpeedRangeBeforeLandingZ.X > SpeedRangeBeforeLandingZ.Y) Swap(SpeedRange2D.X, SpeedRange2D.Y);
	if (VelocityAngleRangePitch.X > VelocityAngleRangePitch.Y) Swap(VelocityAngleRangePitch.X, VelocityAngleRangePitch.Y);
	if (VelocityAngleRangeBeforeLandingYaw.X > VelocityAngleRangeBeforeLandingYaw.Y) Swap(VelocityAngleRangeBeforeLandingYaw.X, VelocityAngleRangeBeforeLandingYaw.Y);

	SpeedRange2D.X = FMath::Max(SpeedRange2D.X, 0.f);
	SpeedRange2D.Y = FMath::Max(SpeedRange2D.Y, 0.f);
	SpeedRangeBeforeLandingZ.X = FMath::Max(SpeedRange2D.X, 0.f);
	SpeedRangeBeforeLandingZ.Y = FMath::Max(SpeedRange2D.Y, 0.f);
	VelocityAngleRangePitch.X = FMath::ClampAngle(VelocityAngleRangePitch.X, -180.f, 180.f);
	VelocityAngleRangePitch.Y = FMath::ClampAngle(VelocityAngleRangePitch.Y, -180.f, 180.f);
	VelocityAngleRangeBeforeLandingYaw.X = FMath::ClampAngle(VelocityAngleRangeBeforeLandingYaw.X, -180.f, 180.f);
	VelocityAngleRangeBeforeLandingYaw.Y = FMath::ClampAngle(VelocityAngleRangeBeforeLandingYaw.Y, -180.f, 180.f);
	
	return IsValid();
}

bool FVSLandRecoveryLimits::Matches(const UVSCharacterMovementFeatureAgent* Agent) const
{
	if (!Agent) return false;;

	const FTransform& CharacterTransformWS = Agent->GetCharacter()->GetActorTransform();
	const FVector& Velocity2DRS = CharacterTransformWS.InverseTransformVector(Agent->GetVelocity2D());
	const FVector& VelocityZRS = CharacterTransformWS.InverseTransformVector(Agent->GetCharacterMovement()->GetLastUpdateVelocity().ProjectOnTo(Agent->GetUpDirection()));
	float VelocityAngleYaw = Velocity2DRS.Rotation().Yaw;

	const float SpeedSignZ = FMath::Sign(VelocityZRS.Dot(Agent->GetUpDirection()) > 0.f);
	float VelocityAnglePitch = 0.f;
	if (VelocityZRS.IsNearlyZero()) VelocityAnglePitch = 0.f;
	else if (Velocity2DRS.IsNearlyZero()) VelocityAnglePitch = -90.f;
	else
	{
		const float Tan = VelocityZRS.Size() / Velocity2DRS.Size();
		VelocityAnglePitch = SpeedSignZ * UKismetMathLibrary::DegAtan(Tan);
	}

	if (!AllowedPrevMovementModes.IsEmpty() && !AllowedPrevMovementModes.Contains(Agent->GetPrevMovementMode())) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(Velocity2DRS.Size(), SpeedRange2D.X, SpeedRange2D.Y)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityZRS.Size(), SpeedRangeBeforeLandingZ.X, SpeedRangeBeforeLandingZ.Y)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAnglePitch, VelocityAngleRangePitch.X, VelocityAngleRangePitch.Y)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAngleYaw, VelocityAngleRangeBeforeLandingYaw.X, VelocityAngleRangeBeforeLandingYaw.Y)) return false;
	
	return true;
}

bool FVSLandRecoverySettings::IsValid() const
{
	if (AnimPlayPolicy == EVSLandRecoveryAnimPlayPolicy::None) return false;
	if (AdditiveAlphaClampBySpeedZ.X < 0.f || AdditiveAlphaClampBySpeedZ.X > AdditiveAlphaClampBySpeedZ.Y
		|| AdditiveAlphaClampBySpeedZ.Z < 0.f || AdditiveAlphaClampBySpeedZ.Z > AdditiveAlphaClampBySpeedZ.W) return false;
	if (!Limits.IsValid()) return false;

	FVSAnimSequenceReference* AnimSequenceReference = RecoveryAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!AnimSequenceReference || !AnimSequenceReference->IsValid()) return false;
	
	return true;
}