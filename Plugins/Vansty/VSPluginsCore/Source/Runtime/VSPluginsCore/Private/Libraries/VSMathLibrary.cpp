// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSMathLibrary.h"

#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UVSMathLibrary::UVSMathLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FRotator UVSMathLibrary::RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal)
{
	const FQuat& WorldToPlaneRotation = UKismetMathLibrary::Quat_FindBetweenVectors(PlaneNormal, FVector::UpVector);
	const FQuat& PlaneToWorldRotation = WorldToPlaneRotation.Inverse();
	FRotator RotationRS = UKismetMathLibrary::ComposeRotators(Rotator, PlaneToWorldRotation.Rotator());
	RotationRS.Roll = 0.f;
	RotationRS.Normalize();
	return UKismetMathLibrary::ComposeRotators(RotationRS, WorldToPlaneRotation.Rotator());
}

FVector UVSMathLibrary::TransformVectorWithScale(const FVector& Vector, const FTransform& Transform)
{
	return Transform.TransformVector(Vector);
}

FVector UVSMathLibrary::InverseTransformVectorWithScale(const FVector& Vector, const FTransform& Transform)
{
	return Transform.InverseTransformVector(Vector);
}

FRotator UVSMathLibrary::RotatorApplyAxes(const FRotator& From, const FRotator& To, EVSRotatorAxes::Type AxesToApply, const FRotator& AxesSpace)
{
	const FTransform& AxesSpaceTransform = FTransform(AxesSpace);
	const FRotator& AxesSpaceFrom = UKismetMathLibrary::InverseTransformRotation(AxesSpaceTransform, From);
	const FRotator& AxesSpaceTo = UKismetMathLibrary::InverseTransformRotation(AxesSpaceTransform, To);
	FRotator AxesSpaceAnsRotation = AxesSpaceFrom;
	
	if (AxesToApply & EVSRotatorAxes::Roll) AxesSpaceAnsRotation.Roll = AxesSpaceTo.Roll;
	if (AxesToApply & EVSRotatorAxes::Pitch) AxesSpaceAnsRotation.Pitch = AxesSpaceTo.Pitch;
	if (AxesToApply & EVSRotatorAxes::Yaw) AxesSpaceAnsRotation.Yaw = AxesSpaceTo.Yaw;

	return UKismetMathLibrary::TransformRotation(AxesSpaceTransform, AxesSpaceAnsRotation);
}

FVector UVSMathLibrary::VectorApplyAxes(const FVector& From, const FVector& To, EVSVectorAxes::Type AxesToApply, const FRotator& AxesSpace)
{
	const FTransform& AxesSpaceTransform = FTransform(AxesSpace);
	const FVector& AxesSpaceFrom = UKismetMathLibrary::InverseTransformDirection(AxesSpaceTransform, From);
	const FVector& AxesSpaceTo = UKismetMathLibrary::InverseTransformDirection(AxesSpaceTransform, To);
	FVector AxesSpaceAnsVector = AxesSpaceFrom;
	
	if (AxesToApply & EVSVectorAxes::X) AxesSpaceAnsVector.X = AxesSpaceTo.X;
	if (AxesToApply & EVSVectorAxes::Y) AxesSpaceAnsVector.Y = AxesSpaceTo.Y;
	if (AxesToApply & EVSVectorAxes::Z) AxesSpaceAnsVector.Z = AxesSpaceTo.Z;

	return UKismetMathLibrary::TransformDirection(AxesSpaceTransform, AxesSpaceAnsVector);
}

FTransform UVSMathLibrary::TransformApplyAxes(const FTransform& From, const FTransform& To, const FVSTransformAxes& AxesToApply, const FRotator& AxesSpace)
{
	const FRotator& AnsRotation = RotatorApplyAxes(From.Rotator(), To.Rotator(), AxesToApply.RotatorAxes, AxesSpace);
	const FVector& AnsTranslation = VectorApplyAxes(From.GetTranslation(), To.GetTranslation(), AxesToApply.TranslationAxes, AxesSpace);
	const FVector& AnsScale = VectorApplyAxes(From.GetScale3D(), To.GetScale3D(), AxesToApply.ScaleAxes, AxesSpace);

	return FTransform(AnsRotation, AnsTranslation, AnsScale);
}

float UVSMathLibrary::FloatInterpTo(const float From, const float To, float DeltaTime, const float LagSpeed, float MaxTimeStep)
{
	float AnsFloat = From;
	
	if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
	{
		float RemainingTime = DeltaTime;
		const float FloatStep = (To - From) * (1.f / DeltaTime);
		float FloatLerpTarget = AnsFloat;

		while (RemainingTime > UE_KINDA_SMALL_NUMBER)
		{
			const float LerpAmount = FMath::Min(MaxTimeStep, RemainingTime);
			RemainingTime -= LerpAmount;
			FloatLerpTarget += FloatStep * LerpAmount;
			
			AnsFloat = UKismetMathLibrary::FInterpTo(AnsFloat, To, LerpAmount, LagSpeed);
		}
	}
	else
	{
		AnsFloat = UKismetMathLibrary::FInterpTo(AnsFloat, To, DeltaTime, LagSpeed);
	}

	return AnsFloat;
}

FRotator UVSMathLibrary::RotatorInterpTo(const FRotator& From, const FRotator& To, float DeltaTime, const FRotator& InterpSpeed, bool bConstantSpeed, float MaxTimeStep, const FRotator& InterpSpace)
{
	const FTransform& LagSpaceTransform = FTransform(InterpSpace);
	const FRotator& LagSpaceFrom = UKismetMathLibrary::InverseTransformRotation(LagSpaceTransform, From);
	const FRotator& LagSpaceTo = UKismetMathLibrary::InverseTransformRotation(LagSpaceTransform, To);
	
	FRotator LagSpaceAnsRotation = LagSpaceFrom;
	
	if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
	{
		float RemainingTime = DeltaTime;

		const FRotator& RotationStep = (LagSpaceTo - LagSpaceFrom).GetNormalized() * (1.f / DeltaTime);

		FRotator RotationLerpTarget = LagSpaceAnsRotation;

		while (RemainingTime > UE_KINDA_SMALL_NUMBER)
		{
			const float LerpAmount = FMath::Min(MaxTimeStep, RemainingTime);
			RemainingTime -= LerpAmount;
			RotationLerpTarget += RotationStep * LerpAmount;

			if (!bConstantSpeed)
			{
				LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Roll).Roll;
				LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Pitch).Pitch;
				LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Yaw).Yaw;
			}
			else
			{
				LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Roll).Roll;
				LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Pitch).Pitch;
				LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Yaw).Yaw;
			}
		}
	}
	else
	{
		if (!bConstantSpeed)
		{
			LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, LagSpaceTo, DeltaTime, InterpSpeed.Roll).Roll;
			LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, LagSpaceTo, DeltaTime, InterpSpeed.Pitch).Pitch;
			LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, LagSpaceTo, DeltaTime, InterpSpeed.Yaw).Yaw;
		}
		else
		{
			LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, LagSpaceTo, DeltaTime, InterpSpeed.Roll).Roll;
			LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, LagSpaceTo, DeltaTime, InterpSpeed.Pitch).Pitch;
			LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, LagSpaceTo, DeltaTime, InterpSpeed.Yaw).Yaw;
		}
	}

	return UKismetMathLibrary::TransformRotation(LagSpaceTransform, LagSpaceAnsRotation);
}

FVector UVSMathLibrary::VectorInterpTo(const FVector& From, const FVector& To, float DeltaTime, const FVector& InterpSpeed, bool bConstantSpeed, float MaxTimeStep, const FRotator& LagSpaceRotation)
{
	const FTransform& LagSpaceTransform = FTransform(LagSpaceRotation);
	const FVector& LagSpaceFrom = UKismetMathLibrary::InverseTransformDirection(LagSpaceTransform, From);
	const FVector& LagSpaceTo = UKismetMathLibrary::InverseTransformDirection(LagSpaceTransform, To);

	FVector LagSpaceAnsVector = LagSpaceFrom;
	
	if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
	{
		float RemainingTime = DeltaTime;

		const FVector& VectorStep = (LagSpaceTo - LagSpaceFrom) * (1.f / DeltaTime);

		FVector VectorLerpTarget = LagSpaceAnsVector;

		while (RemainingTime > UE_KINDA_SMALL_NUMBER)
		{
			const float LerpAmount = FMath::Min(MaxTimeStep, RemainingTime);
			RemainingTime -= LerpAmount;
			VectorLerpTarget += VectorStep * LerpAmount;

			if (!bConstantSpeed)
			{
				LagSpaceAnsVector.X = FMath::FInterpTo(LagSpaceAnsVector.X, VectorLerpTarget.X, LerpAmount, InterpSpeed.X);
				LagSpaceAnsVector.Y = FMath::FInterpTo(LagSpaceAnsVector.Y, VectorLerpTarget.Y, LerpAmount, InterpSpeed.Y);
				LagSpaceAnsVector.Z = FMath::FInterpTo(LagSpaceAnsVector.Z, VectorLerpTarget.Z, LerpAmount, InterpSpeed.Z);
			}
			else
			{
				LagSpaceAnsVector.X = FMath::FInterpConstantTo(LagSpaceAnsVector.X, VectorLerpTarget.X, LerpAmount, InterpSpeed.X);
				LagSpaceAnsVector.Y = FMath::FInterpConstantTo(LagSpaceAnsVector.Y, VectorLerpTarget.Y, LerpAmount, InterpSpeed.Y);
				LagSpaceAnsVector.Z = FMath::FInterpConstantTo(LagSpaceAnsVector.Z, VectorLerpTarget.Z, LerpAmount, InterpSpeed.Z);
			}
		}
	}
	else
	{
		if (!bConstantSpeed)
		{
			LagSpaceAnsVector.X = FMath::FInterpTo(LagSpaceAnsVector.X, LagSpaceTo.X, DeltaTime, InterpSpeed.X);
			LagSpaceAnsVector.Y = FMath::FInterpTo(LagSpaceAnsVector.Y, LagSpaceTo.Y, DeltaTime, InterpSpeed.Y);
			LagSpaceAnsVector.Z = FMath::FInterpTo(LagSpaceAnsVector.Z, LagSpaceTo.Z, DeltaTime, InterpSpeed.Z);
		}
		else
		{
			LagSpaceAnsVector.X = FMath::FInterpConstantTo(LagSpaceAnsVector.X, LagSpaceTo.X, DeltaTime, InterpSpeed.X);
			LagSpaceAnsVector.Y = FMath::FInterpConstantTo(LagSpaceAnsVector.Y, LagSpaceTo.Y, DeltaTime, InterpSpeed.Y);
			LagSpaceAnsVector.Z = FMath::FInterpConstantTo(LagSpaceAnsVector.Z, LagSpaceTo.Z, DeltaTime, InterpSpeed.Z);
		}
	}

	return UKismetMathLibrary::TransformDirection(LagSpaceTransform, LagSpaceAnsVector);
}

FTransform UVSMathLibrary::TransformInterpTo(const FTransform& From, const FTransform& To, float DeltaTime, const FTransform& InterpSpeed, bool bConstantSpeed, float MaxTimeStep, const FRotator& InterpSpace)
{
	const FTransform& LagSpaceTransform = FTransform(InterpSpace);
	const FTransform& LagSpaceFrom = UKismetMathLibrary::MakeRelativeTransform(From, LagSpaceTransform);
	const FTransform& LagSpaceTo = UKismetMathLibrary::MakeRelativeTransform(To, LagSpaceTransform);
	
	FRotator LagSpaceAnsRotation = LagSpaceFrom.Rotator();
	FVector LagSpaceAnsTranslation = LagSpaceFrom.GetTranslation();
	FVector LagSpaceAnsScale = LagSpaceFrom.GetScale3D();
	
	if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
	{
		float RemainingTime = DeltaTime;

		const FRotator& RotationStep = (LagSpaceTo.Rotator() - LagSpaceFrom.Rotator()).GetNormalized() * (1.f / DeltaTime);
		const FVector& TranslationStep = (LagSpaceTo.GetTranslation() - LagSpaceFrom.GetTranslation()) * (1.f / DeltaTime);
		const FVector& ScaleStep = (LagSpaceTo.GetScale3D() - LagSpaceFrom.GetScale3D()) * (1.f / DeltaTime);

		FRotator RotationLerpTarget = LagSpaceAnsRotation;
		FVector TranslationLerpTarget = LagSpaceAnsTranslation;
		FVector ScaleLerpTarget = LagSpaceAnsScale;

		while (RemainingTime > UE_KINDA_SMALL_NUMBER)
		{
			const float LerpAmount = FMath::Min(MaxTimeStep, RemainingTime);
			RemainingTime -= LerpAmount;
			RotationLerpTarget += RotationStep * LerpAmount;
			TranslationLerpTarget += TranslationStep * LerpAmount;
			ScaleLerpTarget += ScaleStep * LerpAmount;

			if (!bConstantSpeed)
			{
				LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Rotator().Roll).Roll;
				LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Rotator().Pitch).Pitch;
				LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Rotator().Yaw).Yaw;

				LagSpaceAnsTranslation.X = FMath::FInterpTo(LagSpaceAnsTranslation.X, TranslationLerpTarget.X, LerpAmount, InterpSpeed.GetTranslation().X);
				LagSpaceAnsTranslation.Y = FMath::FInterpTo(LagSpaceAnsTranslation.Y, TranslationLerpTarget.Y, LerpAmount, InterpSpeed.GetTranslation().Y);
				LagSpaceAnsTranslation.Z = FMath::FInterpTo(LagSpaceAnsTranslation.Z, TranslationLerpTarget.Z, LerpAmount, InterpSpeed.GetTranslation().Z);
			
				LagSpaceAnsScale.X = FMath::FInterpTo(LagSpaceAnsScale.X, ScaleLerpTarget.X, LerpAmount, InterpSpeed.GetScale3D().X);
				LagSpaceAnsScale.Y = FMath::FInterpTo(LagSpaceAnsScale.Y, ScaleLerpTarget.Y, LerpAmount, InterpSpeed.GetScale3D().Y);
				LagSpaceAnsScale.Z = FMath::FInterpTo(LagSpaceAnsScale.Z, ScaleLerpTarget.Z, LerpAmount, InterpSpeed.GetScale3D().Z);
			}
			else
			{
				LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Rotator().Roll).Roll;
				LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Rotator().Pitch).Pitch;
				LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, RotationLerpTarget, LerpAmount, InterpSpeed.Rotator().Yaw).Yaw;

				LagSpaceAnsTranslation.X = FMath::FInterpConstantTo(LagSpaceAnsTranslation.X, TranslationLerpTarget.X, LerpAmount, InterpSpeed.GetTranslation().X);
				LagSpaceAnsTranslation.Y = FMath::FInterpConstantTo(LagSpaceAnsTranslation.Y, TranslationLerpTarget.Y, LerpAmount, InterpSpeed.GetTranslation().Y);
				LagSpaceAnsTranslation.Z = FMath::FInterpConstantTo(LagSpaceAnsTranslation.Z, TranslationLerpTarget.Z, LerpAmount, InterpSpeed.GetTranslation().Z);
			
				LagSpaceAnsScale.X = FMath::FInterpConstantTo(LagSpaceAnsScale.X, ScaleLerpTarget.X, LerpAmount, InterpSpeed.GetScale3D().X);
				LagSpaceAnsScale.Y = FMath::FInterpConstantTo(LagSpaceAnsScale.Y, ScaleLerpTarget.Y, LerpAmount, InterpSpeed.GetScale3D().Y);
				LagSpaceAnsScale.Z = FMath::FInterpConstantTo(LagSpaceAnsScale.Z, ScaleLerpTarget.Z, LerpAmount, InterpSpeed.GetScale3D().Z);
			}
		}
	}
	else
	{
		if (!bConstantSpeed)
		{
			LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, LagSpaceTo.Rotator(), DeltaTime, InterpSpeed.Rotator().Roll).Roll;
			LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, LagSpaceTo.Rotator(), DeltaTime, InterpSpeed.Rotator().Pitch).Pitch;
			LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo(LagSpaceAnsRotation, LagSpaceTo.Rotator(), DeltaTime, InterpSpeed.Rotator().Yaw).Yaw;

			LagSpaceAnsTranslation.X = FMath::FInterpTo(LagSpaceAnsTranslation.X, LagSpaceTo.GetTranslation().X, DeltaTime, InterpSpeed.GetTranslation().X);
			LagSpaceAnsTranslation.Y = FMath::FInterpTo(LagSpaceAnsTranslation.Y, LagSpaceTo.GetTranslation().Y, DeltaTime, InterpSpeed.GetTranslation().Y);
			LagSpaceAnsTranslation.Z = FMath::FInterpTo(LagSpaceAnsTranslation.Z, LagSpaceTo.GetTranslation().Z, DeltaTime, InterpSpeed.GetTranslation().Z);
		
			LagSpaceAnsScale.X = FMath::FInterpTo(LagSpaceAnsScale.X, LagSpaceTo.GetScale3D().X, DeltaTime, InterpSpeed.GetScale3D().X);
			LagSpaceAnsScale.Y = FMath::FInterpTo(LagSpaceAnsScale.Y, LagSpaceTo.GetScale3D().Y, DeltaTime, InterpSpeed.GetScale3D().Y);
			LagSpaceAnsScale.Z = FMath::FInterpTo(LagSpaceAnsScale.Z, LagSpaceTo.GetScale3D().Z, DeltaTime, InterpSpeed.GetScale3D().Z);
		}
		else
		{
			LagSpaceAnsRotation.Roll = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, LagSpaceTo.Rotator(), DeltaTime, InterpSpeed.Rotator().Roll).Roll;
			LagSpaceAnsRotation.Pitch = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, LagSpaceTo.Rotator(), DeltaTime, InterpSpeed.Rotator().Pitch).Pitch;
			LagSpaceAnsRotation.Yaw = UKismetMathLibrary::RInterpTo_Constant(LagSpaceAnsRotation, LagSpaceTo.Rotator(), DeltaTime, InterpSpeed.Rotator().Yaw).Yaw;

			LagSpaceAnsTranslation.X = FMath::FInterpConstantTo(LagSpaceAnsTranslation.X, LagSpaceTo.GetTranslation().X, DeltaTime, InterpSpeed.GetTranslation().X);
			LagSpaceAnsTranslation.Y = FMath::FInterpConstantTo(LagSpaceAnsTranslation.Y, LagSpaceTo.GetTranslation().Y, DeltaTime, InterpSpeed.GetTranslation().Y);
			LagSpaceAnsTranslation.Z = FMath::FInterpConstantTo(LagSpaceAnsTranslation.Z, LagSpaceTo.GetTranslation().Z, DeltaTime, InterpSpeed.GetTranslation().Z);
		
			LagSpaceAnsScale.X = FMath::FInterpConstantTo(LagSpaceAnsScale.X, LagSpaceTo.GetScale3D().X, DeltaTime, InterpSpeed.GetScale3D().X);
			LagSpaceAnsScale.Y = FMath::FInterpConstantTo(LagSpaceAnsScale.Y, LagSpaceTo.GetScale3D().Y, DeltaTime, InterpSpeed.GetScale3D().Y);
			LagSpaceAnsScale.Z = FMath::FInterpConstantTo(LagSpaceAnsScale.Z, LagSpaceTo.GetScale3D().Z, DeltaTime, InterpSpeed.GetScale3D().Z);
		}
	}

	const FTransform& LagSpaceAnsTransform = FTransform(LagSpaceAnsRotation, LagSpaceAnsTranslation, LagSpaceAnsScale);
	return UKismetMathLibrary::ComposeTransforms(LagSpaceAnsTransform, LagSpaceTransform);
}

FVector UVSMathLibrary::ClampVectorTranslation(const FVector& Source, const FVector& RangeMin, const FVector& RangeMax, bool bTranslationAsLocation, const FTransform& ConstrainSpace)
{
	const FVector& SourceTranslationCS = bTranslationAsLocation ? UKismetMathLibrary::InverseTransformLocation(ConstrainSpace, Source) : UKismetMathLibrary::InverseTransformDirection(ConstrainSpace, Source);

	FVector AnsTranslationCS = SourceTranslationCS;
	AnsTranslationCS.X = FMath::Clamp(SourceTranslationCS.X, RangeMin.X, RangeMax.Y);
	AnsTranslationCS.Y = FMath::Clamp(SourceTranslationCS.Y, RangeMin.Y, RangeMax.Y);
	AnsTranslationCS.Z = FMath::Clamp(SourceTranslationCS.Z, RangeMin.Z, RangeMax.Z);

	return bTranslationAsLocation ? UKismetMathLibrary::TransformLocation(ConstrainSpace, AnsTranslationCS) : UKismetMathLibrary::TransformDirection(ConstrainSpace, AnsTranslationCS);
}

FVector UVSMathLibrary::ClampVectorScale(const FVector& Source, const FVector& RangeMin, const FVector& RangeMax, const FTransform& ConstrainSpace)
{
	const FTransform& ScaleTransformWS = FTransform(FQuat::Identity, FVector::ZeroVector, Source);
	const FTransform& ScaleTransformCS = UKismetMathLibrary::MakeRelativeTransform(ScaleTransformWS, ConstrainSpace);
	const FVector& SourceScaleCS = ScaleTransformCS.GetScale3D();
	
	FVector AnsScaleCS = SourceScaleCS;
	AnsScaleCS.X = FMath::Clamp(SourceScaleCS.X, RangeMin.X, RangeMax.Y);
	AnsScaleCS.Y = FMath::Clamp(SourceScaleCS.Y, RangeMin.Y, RangeMax.Y);
	AnsScaleCS.Z = FMath::Clamp(SourceScaleCS.Z, RangeMin.Z, RangeMax.Z);

	const FTransform& AnsTransformCS = FTransform(FQuat::Identity, FVector::ZeroVector, AnsScaleCS);
	return (AnsTransformCS * ConstrainSpace).GetScale3D();
}

FRotator UVSMathLibrary::ClampRotation(const FRotator& Source, const FRotator& RangeMin, const FRotator& RangeMax, const FRotator& ConstrainSpace)
{
	const FTransform& ConstrainSpaceT = FTransform(ConstrainSpace);
	const FRotator& SourceRotationCS = UKismetMathLibrary::InverseTransformRotation(ConstrainSpaceT, Source);

	FRotator AnsRotationCS = SourceRotationCS;
	AnsRotationCS.Roll = FMath::ClampAngle(SourceRotationCS.Roll, RangeMin.Roll, RangeMax.Roll);
	AnsRotationCS.Yaw = FMath::ClampAngle(SourceRotationCS.Yaw, RangeMin.Yaw, RangeMax.Yaw);
	AnsRotationCS.Pitch = FMath::ClampAngle(SourceRotationCS.Pitch, RangeMin.Pitch, RangeMax.Pitch);

	return UKismetMathLibrary::TransformRotation(ConstrainSpaceT, AnsRotationCS);
}

FTransform UVSMathLibrary::ClampTransform(const FTransform& Source, const FTransform& RangeMin, const FTransform& RangeMax, bool bTranslationAsLocation, const FTransform& ConstrainSpace)
{
	const FRotator& AnsRotation = ClampRotation(Source.Rotator(), RangeMin.Rotator(), RangeMax.Rotator(), ConstrainSpace.Rotator());
	const FVector& AnsTranslation = ClampVectorTranslation(Source.GetTranslation(), RangeMin.GetTranslation(), RangeMax.GetTranslation(), bTranslationAsLocation, ConstrainSpace);
	const FVector& AnsScale = ClampVectorScale(Source.GetScale3D(), RangeMin.GetScale3D(), RangeMax.GetScale3D(), ConstrainSpace);
	
	return FTransform(AnsRotation.Quaternion(), AnsTranslation, AnsScale);
}

float UVSMathLibrary::GetVectorDiagonalSize2D(const FVector2D& Vector2D, float AngleYaw)
{
	return FMath::Abs(Vector2D.X * Vector2D.Y / (Vector2D.X * UKismetMathLibrary::DegSin(AngleYaw) + Vector2D.Y * UKismetMathLibrary::DegCos(AngleYaw)));
}

float UVSMathLibrary::GetVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation)
{
	const float SizeXY = GetVectorDiagonalSize2D(FVector2D(Vector.X, Vector.Y), Rotation.Yaw);
	const FVector& Direction = Rotation.Vector();
	const float AngleZ = UKismetMathLibrary::DegAcos(Direction.Z / Direction.Size());
	return GetVectorDiagonalSize2D(FVector2D(Vector.Z, SizeXY), AngleZ);
}