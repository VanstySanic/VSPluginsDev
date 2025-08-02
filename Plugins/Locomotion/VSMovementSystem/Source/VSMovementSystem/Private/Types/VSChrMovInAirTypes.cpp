// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSChrMovInAirTypes.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

bool FVSFixedPointLeapLimits::IsValid() const
{
	if (SpeedRange2D.X < 0.f || SpeedRange2D.X > SpeedRange2D.Y) return false;	
	if (SpeedRangeZ.X > SpeedRangeZ.Y) return false;
	if (VelocityAngleRangePitch.X > VelocityAngleRangePitch.Y) return false;	
	if (VelocityAngleRangeYaw.X > VelocityAngleRangeYaw.Y) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangeYaw.X, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangeYaw.Y, -180.f, 180.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangePitch.X, -90.f, 90.f)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAngleRangePitch.Y, -90.f, 90.f)) return false;
	
	return true;
}

bool FVSFixedPointLeapLimits::Matches(const UVSCharacterMovementFeatureAgent* Agent) const
{
	if (!Agent) return false;

	const FTransform& CharacterTransformWS = Agent->GetCharacter()->GetActorTransform();
	const FVector& Velocity2DRS = CharacterTransformWS.InverseTransformVector(FVector::VectorPlaneProject(Agent->GetVelocity(), Agent->GetUpDirection()));
	const FVector& VelocityZRS = CharacterTransformWS.InverseTransformVector(Agent->GetVelocity().ProjectOnToNormal(Agent->GetUpDirection()));
	float VelocityAngleYaw = Velocity2DRS.Rotation().Yaw;

	const float SpeedSignZ = FMath::Sign(VelocityZRS.Dot(Agent->GetUpDirection()) > 0.f);
	float VelocityAnglePitch = 0.f;
	if (VelocityZRS.IsNearlyZero()) VelocityAnglePitch = 0.f;
	else if (Velocity2DRS.IsNearlyZero()) VelocityAnglePitch = SpeedSignZ * 90.f;
	else
	{
		const float Tan = VelocityZRS.Size() / Velocity2DRS.Size();
		VelocityAnglePitch = SpeedSignZ * UKismetMathLibrary::DegAtan(Tan);
	}

	if (!UKismetMathLibrary::InRange_FloatFloat(Velocity2DRS.Size(), SpeedRange2D.X, SpeedRange2D.Y)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(SpeedSignZ * VelocityZRS.Size(), SpeedRangeZ.X, SpeedRangeZ.Y)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAnglePitch, VelocityAngleRangePitch.X, VelocityAngleRangePitch.Y)) return false;
	if (!UKismetMathLibrary::InRange_FloatFloat(VelocityAngleYaw, VelocityAngleRangeYaw.X, VelocityAngleRangeYaw.Y)) return false;
	if (!MovementTagQuery.IsEmpty() && MovementTagQuery.Matches(Agent->GetGameplayTagController()->GetGameplayTags())) return false;

	return true;
}

bool FVSFixedPointLeapSettings::IsValid() const
{
	if (!Limits.IsValid()) return false;
	if (AnimRows.IsEmpty()) return false;
	
	return true;
}
