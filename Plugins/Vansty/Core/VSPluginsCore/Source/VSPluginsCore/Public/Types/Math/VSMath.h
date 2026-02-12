// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSMathTypes.h"
#include "UObject/Object.h"

/**
 * Math utility type extending FMath with additional helper functions.
 *
 * Provides higher-level math operations commonly used in gameplay code,
 * including safe arithmetic, axis-aware composition, space-relative
 * interpolation, and constrained clamping helpers.
 */
struct VSPLUGINSCORE_API FVSMath : public FMath
{
	template <typename T1, typename T2 = T1, typename T3 = T2>
	// ReSharper disable once CppNotAllPathsReturnValue
	[[nodiscard]] FORCEINLINE static bool IsInRange(const T1& V, const T2& RangeMin, const T3& RangeMax, bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		using NumType = decltype(T1() * T2() * T3());
		static_assert(TIsFloatingPoint<NumType>::Value || TIsIntegral<NumType>::Value, "SafeDivide only supports arithmetic types.");
		
		if (V < RangeMin || (!bIncludeRangeMin && V == RangeMin)) return false;
		if (V > RangeMax || (!bIncludeRangeMax && V == RangeMax)) return false;
		return true;
	}
	
	/**
	 * Safely divides A by B.
	 * - For float/double: returns 0 when B is nearly zero.
	 * - For integers: returns 0 when B is zero.
	 * - For FVector / FVector2D / FVector4: performs component-wise safe division.
	 *
	 * Only supports arithmetic types and UE vector types.
	 */
	template <typename T>
	// ReSharper disable once CppNotAllPathsReturnValue
	[[nodiscard]] FORCEINLINE static T SafeDivide(const T& A, const T& B)
	{
		static_assert(
			TIsFloatingPoint<T>::Value
			|| TIsIntegral<T>::Value
			|| std::is_same_v<T, FIntPoint>
			|| std::is_same_v<T, FVector>
			|| std::is_same_v<T, FVector2D>
			|| std::is_same_v<T, FVector4>,
			"SafeDivide only supports arithmetic types, FIntPoint, FVector, FVector2D and FVector4.");
		
		/** Vector types: component-wise safe division. */
		if constexpr (std::is_same_v<T, FIntPoint>)
		{
			return FIntPoint(SafeDivide(A.X, B.X), SafeDivide(A.Y, B.Y));
		}
		else if constexpr (std::is_same_v<T, FVector2D>)
		{
			return FVector2D(SafeDivide(A.X, B.X), SafeDivide(A.Y, B.Y));
		}
		else if constexpr (std::is_same_v<T, FVector>)
		{
			return FVector(SafeDivide(A.X, B.X), SafeDivide(A.Y, B.Y), SafeDivide(A.Z, B.Z));
		}
		else if constexpr (std::is_same_v<T, FVector4>)
		{
			return FVector4(SafeDivide(A.X, B.X),SafeDivide(A.Y, B.Y), SafeDivide(A.Z, B.Z), SafeDivide(A.W, B.W));
		}
		/** Floating point types: use nearly-zero check. */
		else if constexpr (TIsFloatingPoint<T>::Value)
		{
			return FMath::IsNearlyZero(B) ? static_cast<T>(0) : A / B;
		}
		/** Integral types: exact zero check. */
		else if constexpr (TIsIntegral<T>::Value)
		{
			return (B == static_cast<T>(0)) ? static_cast<T>(0) : A / B;
		}
	}
	
	/** Check if a vector has any axis nearly zero within tolerance. */
	template <typename T>
	// ReSharper disable once CppNotAllPathsReturnValue
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const T& Vector, const double Tolerance = 0.00000001)
	{
		static_assert(
			std::is_same_v<T, FIntPoint>
			|| std::is_same_v<T, FVector>
			|| std::is_same_v<T, FVector2D>
			|| std::is_same_v<T, FVector4>,
			"SafeDivide only supports vector types.");

		/** Vector types: component-wise safe division. */
		if constexpr (std::is_same_v<T, FIntPoint>)
		{
			return Vector.X == 0 || Vector.Y == 0;;
		}
		if constexpr (std::is_same_v<T, FVector2D>)
		{
			return FMath::IsNearlyZero(Vector.X, Tolerance) || FMath::IsNearlyZero(Vector.Y, Tolerance);
		}
		else if constexpr (std::is_same_v<T, FVector>)
		{
			return FMath::IsNearlyZero(Vector.X, Tolerance) || FMath::IsNearlyZero(Vector.Y, Tolerance) || FMath::IsNearlyZero(Vector.Z, Tolerance);
		}
		else if constexpr (std::is_same_v<T, FVector4>)
		{
			return FMath::IsNearlyZero(Vector.X, Tolerance) || FMath::IsNearlyZero(Vector.Y, Tolerance)
			|| FMath::IsNearlyZero(Vector.Z, Tolerance) || FMath::IsNearlyZero(Vector.W, Tolerance);
		}
	}

	FORCEINLINE static FRotator ComposeRotators(const FRotator& RotatorA, const FRotator& RotatorB)
	{
		const FQuat AQuat = FQuat(RotatorA);
		const FQuat BQuat = FQuat(RotatorB);
		return FRotator(BQuat * AQuat);
	}
	
	/** Rearrange or swap vector axes according to swizzle rule. */
	[[nodiscard]] FORCEINLINE static FVector VectorSwizzleAxes(const FVector& Vector, EVSAxesSwizzle::Type Swizzle = EVSAxesSwizzle::XYZ)
	{
		FVector Value = Vector;
		switch (Swizzle)
		{
		case EVSAxesSwizzle::YXZ:
			Swap(Value.X, Value.Y);
			break;
		
		case EVSAxesSwizzle::ZYX:
			Swap(Value.X, Value.Z);
			break;
		
		case EVSAxesSwizzle::XZY:
			Swap(Value.Y, Value.Z);
			break;
		
		case EVSAxesSwizzle::YZX:
			Value = FVector(Value.Y, Value.Z, Value.X);
			break;
		
		case EVSAxesSwizzle::ZXY:
			Value = FVector(Value.Z, Value.X, Value.Y);
			break;

		default:;
		}
		return Value;
	}

	/** Project a rotator orientation onto a plane defined by a normal. */
	[[nodiscard]] FORCEINLINE static FRotator RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal)
	{
		if (PlaneNormal.IsNearlyZero()) return Rotator;
		
		const FQuat WorldToPlaneRotation = FQuat::FindBetweenVectors(PlaneNormal, FVector::UpVector);
		const FQuat PlaneToWorldRotation = WorldToPlaneRotation.Inverse();
		FRotator RotationRS = ComposeRotators(Rotator, PlaneToWorldRotation.Rotator());
		RotationRS.Roll = 0.f;
		RotationRS.Normalize();
		return ComposeRotators(RotationRS, WorldToPlaneRotation.Rotator());
	}

	/**
	 * Make a rotator from source, applying target rotator axes in specified space.
	 * @param AxesSpace Axes will be applied in that space.
	 */
	[[nodiscard]] FORCEINLINE static FRotator RotatorApplyAxes(
		const FRotator& From,
		const FRotator& To,
		EVSRotatorAxes::Type AxesToApply = EVSRotatorAxes::Type::RollPitchYaw,
		const FRotator& AxesSpace = FRotator::ZeroRotator)
	{
		const FTransform AxesSpaceTransform = FTransform(AxesSpace);
		const FRotator AxesSpaceFrom = AxesSpaceTransform.InverseTransformRotation(From.Quaternion()).Rotator();
		const FRotator AxesSpaceTo = AxesSpaceTransform.InverseTransformRotation(To.Quaternion()).Rotator();
		FRotator AxesSpaceAnsRotation = AxesSpaceFrom;
	
		if (AxesToApply & EVSRotatorAxes::Roll) AxesSpaceAnsRotation.Roll = AxesSpaceTo.Roll;
		if (AxesToApply & EVSRotatorAxes::Pitch) AxesSpaceAnsRotation.Pitch = AxesSpaceTo.Pitch;
		if (AxesToApply & EVSRotatorAxes::Yaw) AxesSpaceAnsRotation.Yaw = AxesSpaceTo.Yaw;

		return AxesSpaceTransform.TransformRotation(AxesSpaceAnsRotation.Quaternion()).Rotator();
	}
	
	/**
	 * Make a vector from source, applying To target axes in specified space.
	 * @param AxesSpace Axes will be applied in that space.
	 */
	[[nodiscard]] FORCEINLINE static FVector VectorApplyAxes(
		const FVector& From,
		const FVector& To,
		EVSVectorAxes::Type AxesToApply = EVSVectorAxes::Type::XYZ,
		const FRotator& AxesSpace = FRotator::ZeroRotator)
	{
		const FTransform AxesSpaceTransform = FTransform(AxesSpace);
		const FVector AxesSpaceFrom = AxesSpaceTransform.InverseTransformVectorNoScale(From);
		const FVector AxesSpaceTo = AxesSpaceTransform.InverseTransformVectorNoScale(To);
		FVector AxesSpaceAnsVector = AxesSpaceFrom;
	
		if (AxesToApply & EVSVectorAxes::X) AxesSpaceAnsVector.X = AxesSpaceTo.X;
		if (AxesToApply & EVSVectorAxes::Y) AxesSpaceAnsVector.Y = AxesSpaceTo.Y;
		if (AxesToApply & EVSVectorAxes::Z) AxesSpaceAnsVector.Z = AxesSpaceTo.Z;

		return AxesSpaceTransform.TransformVectorNoScale(AxesSpaceAnsVector);
	}
	
	/**
	 * Make a transform from source, applying target transform axes in specified space.
	 * @param AxesSpace Axes will be applied in that space.
	 */
	[[nodiscard]] FORCEINLINE static FTransform TransformApplyAxes(
		const FTransform& From,
		const FTransform& To,
		const FVSTransformAxes& AxesToApply = FVSTransformAxes(),
		const FRotator& AxesSpace = FRotator::ZeroRotator)
	{
		const FRotator AnsRotation = RotatorApplyAxes(From.Rotator(), To.Rotator(), AxesToApply.RotatorAxes, AxesSpace);
		const FVector AnsTranslation = VectorApplyAxes(From.GetTranslation(), To.GetTranslation(), AxesToApply.TranslationAxes, AxesSpace);
		const FVector AnsScale = VectorApplyAxes(From.GetScale3D(), To.GetScale3D(), AxesToApply.ScaleAxes, AxesSpace);

		return FTransform(AnsRotation, AnsTranslation, AnsScale);
	}

	/**
	 * Interpolate float from Current to Target with sub time-stepping. Scaled by distance to Target, so it has a strong start speed and ease out.
	*  @param InterpSpeed Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	template<typename T1, typename T2 = T1, typename T3 = T2, typename T4 = T3, typename T5 = T4>
	[[nodiscard]] FORCEINLINE static T1 FInterpToAdvanced(
		const T1& From,
		const T2& To,
		T3 DeltaTime,
		const T4 InterpSpeed = 10.f,
		T5 MaxTimeStep = 0.016667f)
	{
		static_assert(!std::is_same_v<T1, bool> && !std::is_same_v<T2, bool>, "Boolean types may not be interpolated");
		using RetType = decltype(T1() * T2() * T3() * T4() * T5());
		static_assert(TIsArithmetic<RetType>::Value, "FInterpToAdvanced only supports arithmetic types.");
		
		RetType Ans = From;
	
		if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
		{
			float RemainingTime = DeltaTime;
			const RetType Step = (To - From) * (static_cast<T1>(1) / DeltaTime);
			RetType LerpTarget = Ans;

			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpTimeAmount = Min(MaxTimeStep, RemainingTime);
				RemainingTime -= LerpTimeAmount;
				LerpTarget += Step * LerpTimeAmount;
			
				Ans = FInterpTo(Ans, LerpTarget, LerpTimeAmount, InterpSpeed);
			}
		}
		else
		{
			Ans = FInterpTo(Ans, To, DeltaTime, InterpSpeed);
		}

		return Ans;
	}
	
	/**
	 * Interpolate a rotator in specified space with optional time sub-stepping.
	 * @param InterpSpeed Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	[[nodiscard]] FORCEINLINE static FRotator RInterpToAdvanced(
		const FRotator& From,
		const FRotator& To,
		float DeltaTime,
		const FRotator& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator)
	{
		const FTransform LagSpaceTransform = FTransform(InterpSpace);
		const FRotator LagSpaceFrom = LagSpaceTransform.InverseTransformRotation(From.Quaternion()).Rotator();
		const FRotator LagSpaceTo = LagSpaceTransform.InverseTransformRotation(To.Quaternion()).Rotator();
		
		FRotator LagSpaceAns = LagSpaceFrom;
		
		if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
		{
			float RemainingTime = DeltaTime;
			const FRotator Step = (LagSpaceTo - LagSpaceFrom).GetNormalized() * (1.0 / DeltaTime);
			FRotator LerpTarget = LagSpaceAns;

			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpTimeAmount = Min(MaxTimeStep, RemainingTime);
				RemainingTime -= LerpTimeAmount;
				LerpTarget += Step * LerpTimeAmount;

				LagSpaceAns.Roll = RInterpTo(LagSpaceAns, LerpTarget, LerpTimeAmount, InterpSpeed.Roll).Roll;
				LagSpaceAns.Pitch = RInterpTo(LagSpaceAns, LerpTarget, LerpTimeAmount, InterpSpeed.Pitch).Pitch;
				LagSpaceAns.Yaw = RInterpTo(LagSpaceAns, LerpTarget, LerpTimeAmount, InterpSpeed.Yaw).Yaw;
			}
		}
		else
		{
			LagSpaceAns.Roll = RInterpTo(LagSpaceAns, LagSpaceTo, DeltaTime, InterpSpeed.Roll).Roll;
			LagSpaceAns.Pitch = RInterpTo(LagSpaceAns, LagSpaceTo, DeltaTime, InterpSpeed.Pitch).Pitch;
			LagSpaceAns.Yaw = RInterpTo(LagSpaceAns, LagSpaceTo, DeltaTime, InterpSpeed.Yaw).Yaw;
		}

		return LagSpaceTransform.TransformRotation(LagSpaceAns.Quaternion()).Rotator();
	}

	[[nodiscard]] FORCEINLINE static FVector2D VInterpTo2DAdvanced(
		const FVector2D& From,
		const FVector2D& To,
		float DeltaTime,
		const FVector2D& InterpSpeed,
		float MaxTimeStep = 0.016667f)
	{
		FVector2D Ans = From;
		if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
		{
			float RemainingTime = DeltaTime;
			const FVector2D Step = (To - From) * (1.0 / DeltaTime);
			FVector2D LerpTarget = Ans;

			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpTimeAmount = Min(MaxTimeStep, RemainingTime);
				RemainingTime -= LerpTimeAmount;
				LerpTarget += Step * LerpTimeAmount;

				Ans.X = FInterpTo(Ans.X, LerpTarget.X, LerpTimeAmount, InterpSpeed.X);
				Ans.Y = FInterpTo(Ans.Y, LerpTarget.Y, LerpTimeAmount, InterpSpeed.Y);
			}
		}
		else
		{
			Ans.X = FInterpTo(Ans.X, To.X, DeltaTime, InterpSpeed.X);
			Ans.Y = FInterpTo(Ans.Y, To.Y, DeltaTime, InterpSpeed.Y);
		}

		return Ans;
	}
	
	/**
	 * Interpolate a vector in specified space with optional time sub-stepping.
	 * @param InterpSpace Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	[[nodiscard]] FORCEINLINE static FVector VInterpToAdvanced(
		const FVector& From,
		const FVector& To,
		float DeltaTime,
		const FVector& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator)
	{
		const FTransform LagSpaceTransform = FTransform(InterpSpace);
		const FVector LagSpaceFrom = LagSpaceTransform.InverseTransformVectorNoScale(From);
		const FVector LagSpaceTo = LagSpaceTransform.InverseTransformVectorNoScale(To);

		FVector LagSpaceAns = LagSpaceFrom;
		
		if (MaxTimeStep > 0.f && DeltaTime > MaxTimeStep)
		{
			float RemainingTime = DeltaTime;
			const FVector Step = (LagSpaceTo - LagSpaceFrom) * (1.0 / DeltaTime);
			FVector LerpTarget = LagSpaceAns;

			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpTimeAmount = Min(MaxTimeStep, RemainingTime);
				RemainingTime -= LerpTimeAmount;
				LerpTarget += Step * LerpTimeAmount;

				LagSpaceAns.X = FInterpTo(LagSpaceAns.X, LerpTarget.X, LerpTimeAmount, InterpSpeed.X);
				LagSpaceAns.Y = FInterpTo(LagSpaceAns.Y, LerpTarget.Y, LerpTimeAmount, InterpSpeed.Y);
				LagSpaceAns.Z = FInterpTo(LagSpaceAns.Z, LerpTarget.Z, LerpTimeAmount, InterpSpeed.Z);
			}
		}
		else
		{
			LagSpaceAns.X = FInterpTo(LagSpaceAns.X, LagSpaceTo.X, DeltaTime, InterpSpeed.X);
			LagSpaceAns.Y = FInterpTo(LagSpaceAns.Y, LagSpaceTo.Y, DeltaTime, InterpSpeed.Y);
			LagSpaceAns.Z = FInterpTo(LagSpaceAns.Z, LagSpaceTo.Z, DeltaTime, InterpSpeed.Z);
		}

		return LagSpaceTransform.TransformVectorNoScale(LagSpaceAns);
	}
	
	/**
	 * Interpolate a transform in specified space with optional time sub-stepping.
	 * @param InterpSpace Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	[[nodiscard]] FORCEINLINE static FTransform TransformInterpTo(
		const FTransform& From,
		const FTransform& To,
		float DeltaTime,
		const FTransform& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator)
	{
		/** Rotation: use RotatorInterpTo with per-axis rotation speeds. */
		const FRotator RotFrom = From.Rotator();
		const FRotator RotTo = To.Rotator();
		const FRotator RotSpeed = InterpSpeed.Rotator();
		const FRotator AnsRot = RInterpToAdvanced(RotFrom, RotTo, DeltaTime, RotSpeed, MaxTimeStep, InterpSpace);

		/** Translation: use VectorInterpTo with per-axis translation speeds. */
		const FVector TransFrom = From.GetTranslation();
		const FVector TransTo = To.GetTranslation();
		const FVector TransSpeed = InterpSpeed.GetTranslation();
		const FVector AnsTrans = VInterpToAdvanced(TransFrom, TransTo, DeltaTime, TransSpeed, MaxTimeStep, InterpSpace);

		/** Scale: use VectorInterpTo with per-axis scale speeds. */
		const FVector ScaleFrom = From.GetScale3D();
		const FVector ScaleTo = To.GetScale3D();
		const FVector ScaleSpeed = InterpSpeed.GetScale3D();
		const FVector AnsScale = VInterpToAdvanced( ScaleFrom, ScaleTo, DeltaTime, ScaleSpeed, MaxTimeStep, InterpSpace);

		return FTransform(AnsRot, AnsTrans, AnsScale);
	}
	
	[[nodiscard]] FORCEINLINE static FVector2D ClampVector2D(
		const FVector2D& Source,
		const FVector2D& RangeMin,
		const FVector2D& RangeMax)
	{
		FVector2D Ans = Source;
		Ans.X = Clamp(Ans.X, RangeMin.X, RangeMax.X);
		Ans.Y = Clamp(Ans.Y, RangeMin.Y, RangeMax.Y);

		return Ans;
	}
	
	[[nodiscard]] FORCEINLINE static FVector ClampVectorLocation(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FTransform& ConstrainSpace = FTransform::Identity)
	{
		const FVector SourceTranslationCS = ConstrainSpace.InverseTransformPosition(Source);

		FVector AnsTranslationCS = SourceTranslationCS;
		AnsTranslationCS.X = Clamp(SourceTranslationCS.X, RangeMin.X, RangeMax.X);
		AnsTranslationCS.Y = Clamp(SourceTranslationCS.Y, RangeMin.Y, RangeMax.Y);
		AnsTranslationCS.Z = Clamp(SourceTranslationCS.Z, RangeMin.Z, RangeMax.Z);

		return ConstrainSpace.TransformPosition(AnsTranslationCS);
	}
	
	[[nodiscard]] FORCEINLINE static FVector ClampVectorDirection(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FRotator& ConstrainSpace = FRotator::ZeroRotator)
	{
		const FTransform ConstrainSpaceTransform = FTransform(ConstrainSpace);
		const FVector SourceTranslationCS = ConstrainSpaceTransform.InverseTransformVectorNoScale(Source);

		FVector AnsTranslationCS = SourceTranslationCS;
		AnsTranslationCS.X = Clamp(SourceTranslationCS.X, RangeMin.X, RangeMax.X);
		AnsTranslationCS.Y = Clamp(SourceTranslationCS.Y, RangeMin.Y, RangeMax.Y);
		AnsTranslationCS.Z = Clamp(SourceTranslationCS.Z, RangeMin.Z, RangeMax.Z);

		return ConstrainSpaceTransform.TransformVectorNoScale(AnsTranslationCS);
	}
	
	[[nodiscard]] FORCEINLINE static FVector ClampVectorScale(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FTransform& ConstrainSpace)
	{
		const FTransform ScaleTransformWS = FTransform(FQuat::Identity, FVector::ZeroVector, Source);
		const FTransform ScaleTransformCS = ScaleTransformWS.GetRelativeTransform(ConstrainSpace);
		const FVector SourceScaleCS = ScaleTransformCS.GetScale3D();
	
		FVector AnsScaleCS = SourceScaleCS;
		AnsScaleCS.X = Clamp(SourceScaleCS.X, RangeMin.X, RangeMax.X);
		AnsScaleCS.Y = Clamp(SourceScaleCS.Y, RangeMin.Y, RangeMax.Y);
		AnsScaleCS.Z = Clamp(SourceScaleCS.Z, RangeMin.Z, RangeMax.Z);

		const FTransform AnsTransformCS = FTransform(FQuat::Identity, FVector::ZeroVector, AnsScaleCS);
		return (AnsTransformCS * ConstrainSpace).GetScale3D();
	}
	
	/** Clamp rotation angle. Range min larger than range max indicates the inversed area. */
	[[nodiscard]] FORCEINLINE static FRotator ClampRotator(
		const FRotator& Source,
		const FRotator& RangeMin,
		const FRotator& RangeMax,
		const FRotator& ConstrainSpace = FRotator::ZeroRotator)
	{
		const FTransform ConstrainSpaceT = FTransform(ConstrainSpace);
		const FRotator SourceRotationCS = ConstrainSpaceT.InverseTransformRotation(Source.Quaternion()).Rotator();

		FRotator AnsRotationCS = SourceRotationCS;
		AnsRotationCS.Roll = ClampAngle(SourceRotationCS.Roll, RangeMin.Roll, RangeMax.Roll);
		AnsRotationCS.Yaw = ClampAngle(SourceRotationCS.Yaw, RangeMin.Yaw, RangeMax.Yaw);
		AnsRotationCS.Pitch = ClampAngle(SourceRotationCS.Pitch, RangeMin.Pitch, RangeMax.Pitch);

		return ConstrainSpaceT.TransformRotation(AnsRotationCS.Quaternion()).Rotator();
	}

	/** Clamp transform for all axes. Rotation range min larger than range max means inverse. */
	[[nodiscard]] FORCEINLINE static FTransform ClampTransform(
		const FTransform& Source,
		const FTransform& RangeMin,
		const FTransform& RangeMax,
		const FTransform& ConstrainSpace = FTransform::Identity)
	{
		const FRotator AnsRotation = ClampRotator(Source.Rotator(), RangeMin.Rotator(), RangeMax.Rotator(), ConstrainSpace.Rotator());
		const FVector AnsTranslation = ClampVectorLocation(Source.GetTranslation(), RangeMin.GetTranslation(), RangeMax.GetTranslation(), ConstrainSpace);
		const FVector AnsScale = ClampVectorScale(Source.GetScale3D(), RangeMin.GetScale3D(), RangeMax.GetScale3D(), ConstrainSpace);
	
		return FTransform(AnsRotation.Quaternion(), AnsTranslation, AnsScale);
	}
	
	/** Get diagonal size of a 2D vector along specified yaw angle. */
	[[nodiscard]] FORCEINLINE static double CalcVector2DDiagonalSize(const FVector2D& Vector2D, float AngleYaw)
	{
		return Abs(SafeDivide(Vector2D.X * Vector2D.Y, (Vector2D.X * Sin(UE_DOUBLE_PI / (180.0) * AngleYaw)) + Vector2D.Y * Cos(UE_DOUBLE_PI / (180.0) * AngleYaw)));
	}
	
	/** Get diagonal size of a 3D vector along specified rotation. */
	[[nodiscard]] FORCEINLINE static double CalcVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation)
	{
		const float SizeXY = CalcVector2DDiagonalSize(FVector2D(Vector.X, Vector.Y), Rotation.Yaw);
		const FVector Direction = Rotation.Vector();
		const float AngleZ = (180.0) / UE_DOUBLE_PI * Acos(SafeDivide(Direction.Z, Direction.Size()));
		return CalcVector2DDiagonalSize(FVector2D(Vector.Z, SizeXY), AngleZ);
	}
};
