// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSMathTypes.h"
#include "UObject/Object.h"
#include <type_traits>

namespace VSMathPrivate
{
	template <typename T>
	struct TIsNumeric : std::bool_constant<TIsFloatingPoint<T>::Value || TIsIntegral<T>::Value || std::is_enum_v<T>>
	{
	};

	template <typename T, bool bIsEnum = std::is_enum_v<T>>
	struct TEnumOrSelf
	{
		using Type = T;
	};

	template <typename T>
	struct TEnumOrSelf<T, true>
	{
		using Type = std::underlying_type_t<T>;
	};

	template <typename T>
	using TEnumOrSelf_t = typename TEnumOrSelf<T>::Type;

	template <typename T>
	struct TIsVector2 : std::false_type
	{
	};
	
	template <typename T>
	struct TIsVector2<UE::Math::TIntPoint<T>> : std::true_type
	{
	};
	
	template <typename T>
	struct TIsVector2<UE::Math::TIntVector2<T>> : std::true_type
	{
	};

	template <typename T>
	struct TIsVector2<UE::Math::TVector2<T>> : std::true_type
	{
	};

	template <typename T>
	struct TIsVector3 : std::false_type
	{
	};

	template <typename T>
	struct TIsVector3<UE::Math::TVector<T>> : std::true_type
	{
	};

	template <typename T>
	struct TIsVector3<UE::Math::TIntVector3<T>> : std::true_type
	{
	};

	template <typename T>
	struct TIsVector4 : std::false_type
	{
	};

	template <typename T>
	struct TIsVector4<UE::Math::TVector4<T>> : std::true_type
	{
	};

	template <typename T>
	struct TIsVector4<UE::Math::TIntVector4<T>> : std::true_type
	{
	};
}

/**
 * Math helper type extending FMath.
 */
struct VSPLUGINSCORE_API FVSMath : public FMath
{
	/** Returns whether V is inside [RangeMin, RangeMax] with configurable inclusive bounds. */
	template <typename T1, typename T2 = T1, typename T3 = T2>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const T1& V, const T2& RangeMin, const T3& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T1>::value && VSMathPrivate::TIsNumeric<T2>::value && VSMathPrivate::TIsNumeric<T3>::value,
			"IsInRange constraints: numeric overload requires numeric T1/T2/T3.");

		using C1 = VSMathPrivate::TEnumOrSelf_t<T1>;
		using C2 = VSMathPrivate::TEnumOrSelf_t<T2>;
		using C3 = VSMathPrivate::TEnumOrSelf_t<T3>;
		using CompareType = decltype(C1() + C2() + C3());

		const CompareType Value = static_cast<CompareType>(V);
		const CompareType MinValue = static_cast<CompareType>(RangeMin);
		const CompareType MaxValue = static_cast<CompareType>(RangeMax);
		if (Value < MinValue || (!bIncludeRangeMin && Value == MinValue)) return false;
		if (Value > MaxValue || (!bIncludeRangeMax && Value == MaxValue)) return false;
		return true;
	}

	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TIntPoint<T>& V, const UE::Math::TIntPoint<T2>& RangeMin, const UE::Math::TIntPoint<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax);
	}

	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TIntVector2<T>& V, const UE::Math::TIntVector2<T2>& RangeMin, const UE::Math::TIntVector2<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax);
	}

	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TVector2<T>& V, const UE::Math::TVector2<T2>& RangeMin, const UE::Math::TVector2<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax);
	}

	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TIntVector3<T>& V, const UE::Math::TIntVector3<T2>& RangeMin, const UE::Math::TIntVector3<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Z, RangeMin.Z, RangeMax.Z, bIncludeRangeMin, bIncludeRangeMax);
	}

	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TVector<T>& V, const UE::Math::TVector<T2>& RangeMin, const UE::Math::TVector<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Z, RangeMin.Z, RangeMax.Z, bIncludeRangeMin, bIncludeRangeMax);
	}
	
	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TIntVector4<T>& V, const UE::Math::TIntVector4<T2>& RangeMin, const UE::Math::TIntVector4<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Z, RangeMin.Z, RangeMax.Z, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.W, RangeMin.W, RangeMax.W, bIncludeRangeMin, bIncludeRangeMax);
	}

	template <typename T, typename T2, typename T3>
	[[nodiscard]] FORCEINLINE static bool IsInRange(
		const UE::Math::TVector4<T>& V, const UE::Math::TVector4<T2>& RangeMin, const UE::Math::TVector4<T3>& RangeMax,
		bool bIncludeRangeMin = true, bool bIncludeRangeMax = true)
	{
		return IsInRange(V.X, RangeMin.X, RangeMax.X, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Y, RangeMin.Y, RangeMax.Y, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.Z, RangeMin.Z, RangeMax.Z, bIncludeRangeMin, bIncludeRangeMax)
			&& IsInRange(V.W, RangeMin.W, RangeMax.W, bIncludeRangeMin, bIncludeRangeMax);
	}
	

	/**
	 * Safely divides A by B and returns T1.
	 * - Numeric T1: T2 must also be numeric.
	 * - Vector T1: T2 must be numeric, or a vector with the same axis count.
	 * - Floating denominator: returns 0 when denominator is nearly zero.
	 * - Integral denominator: returns 0 when denominator is exactly zero.
	 */
	template <typename T1, typename T2 = T1>
	[[nodiscard]] FORCEINLINE static T1 SafeDivide(const T1& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T1>::value && VSMathPrivate::TIsNumeric<T2>::value,
			"SafeDivide constraints: numeric overload requires numeric T1/T2.");

		if constexpr (std::is_enum_v<T1> || std::is_enum_v<T2>)
		{
			using AType = VSMathPrivate::TEnumOrSelf_t<T1>;
			using BType = VSMathPrivate::TEnumOrSelf_t<T2>;
			const BType Divisor = static_cast<BType>(B);
			if constexpr (TIsFloatingPoint<BType>::Value)
			{
				return FMath::IsNearlyZero(static_cast<double>(Divisor), Tolerance)
					? static_cast<T1>(0)
					: static_cast<T1>(static_cast<AType>(A) / Divisor);
			}
			else
			{
				return (Divisor == static_cast<BType>(0))
					? static_cast<T1>(0)
					: static_cast<T1>(static_cast<AType>(A) / Divisor);
			}
		}
		else if constexpr (TIsFloatingPoint<T2>::Value)
		{
			return FMath::IsNearlyZero(static_cast<double>(B), Tolerance) ? static_cast<T1>(0) : static_cast<T1>(A / B);
		}
		else
		{
			return (B == static_cast<T2>(0)) ? static_cast<T1>(0) : static_cast<T1>(A / B);
		}
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntPoint<T> SafeDivide(
		const UE::Math::TIntPoint<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T2>::value || VSMathPrivate::TIsVector2<T2>::value,
			"SafeDivide constraints: TIntPoint divisor must be numeric or Vector2.");

		if constexpr (VSMathPrivate::TIsNumeric<T2>::value)
		{
			return UE::Math::TIntPoint<T>(SafeDivide(A.X, B, Tolerance), SafeDivide(A.Y, B, Tolerance));
		}
		else
		{
			return UE::Math::TIntPoint<T>(SafeDivide(A.X, B.X, Tolerance), SafeDivide(A.Y, B.Y, Tolerance));
		}
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector2<T> SafeDivide(
		const UE::Math::TIntVector2<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		const UE::Math::TIntPoint<T> Ans = SafeDivide(UE::Math::TIntPoint<T>(A.X, A.Y), B, Tolerance);
		return UE::Math::TIntVector2<T>(Ans.X, Ans.Y);
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector2<T> SafeDivide(
		const UE::Math::TVector2<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T2>::value || VSMathPrivate::TIsVector2<T2>::value,
			"SafeDivide constraints: TVector2 divisor must be numeric or Vector2.");

		if constexpr (VSMathPrivate::TIsNumeric<T2>::value)
		{
			return UE::Math::TVector2<T>(SafeDivide(A.X, B, Tolerance), SafeDivide(A.Y, B, Tolerance));
		}
		else
		{
			return UE::Math::TVector2<T>(SafeDivide(A.X, B.X, Tolerance), SafeDivide(A.Y, B.Y, Tolerance));
		}
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector3<T> SafeDivide(
		const UE::Math::TIntVector3<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T2>::value || VSMathPrivate::TIsVector3<T2>::value,
			"SafeDivide constraints: TIntVector3 divisor must be numeric or Vector3.");

		if constexpr (VSMathPrivate::TIsNumeric<T2>::value)
		{
			return UE::Math::TIntVector3<T>(SafeDivide(A.X, B, Tolerance), SafeDivide(A.Y, B, Tolerance), SafeDivide(A.Z, B, Tolerance));
		}
		else
		{
			return UE::Math::TIntVector3<T>(SafeDivide(A.X, B.X, Tolerance), SafeDivide(A.Y, B.Y, Tolerance), SafeDivide(A.Z, B.Z, Tolerance));
		}
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector<T> SafeDivide(
		const UE::Math::TVector<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T2>::value || VSMathPrivate::TIsVector3<T2>::value,
			"SafeDivide constraints: TVector divisor must be numeric or Vector3.");

		if constexpr (VSMathPrivate::TIsNumeric<T2>::value)
		{
			return UE::Math::TVector<T>(SafeDivide(A.X, B, Tolerance), SafeDivide(A.Y, B, Tolerance), SafeDivide(A.Z, B, Tolerance));
		}
		else
		{
			return UE::Math::TVector<T>(SafeDivide(A.X, B.X, Tolerance), SafeDivide(A.Y, B.Y, Tolerance), SafeDivide(A.Z, B.Z, Tolerance));
		}
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector4<T> SafeDivide(
		const UE::Math::TIntVector4<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T2>::value || VSMathPrivate::TIsVector4<T2>::value,
			"SafeDivide constraints: TIntVector4 divisor must be numeric or Vector4.");

		if constexpr (VSMathPrivate::TIsNumeric<T2>::value)
		{
			return UE::Math::TIntVector4<T>(SafeDivide(A.X, B, Tolerance), SafeDivide(A.Y, B, Tolerance), SafeDivide(A.Z, B, Tolerance), SafeDivide(A.W, B, Tolerance));
		}
		else
		{
			return UE::Math::TIntVector4<T>(SafeDivide(A.X, B.X, Tolerance), SafeDivide(A.Y, B.Y, Tolerance), SafeDivide(A.Z, B.Z, Tolerance), SafeDivide(A.W, B.W, Tolerance));
		}
	}

	template <typename T, typename T2>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector4<T> SafeDivide(
		const UE::Math::TVector4<T>& A, const T2& B, const double Tolerance = 1e-8)
	{
		static_assert(
			VSMathPrivate::TIsNumeric<T2>::value || VSMathPrivate::TIsVector4<T2>::value,
			"SafeDivide constraints: TVector4 divisor must be numeric or Vector4.");

		if constexpr (VSMathPrivate::TIsNumeric<T2>::value)
		{
			return UE::Math::TVector4<T>(SafeDivide(A.X, B, Tolerance), SafeDivide(A.Y, B, Tolerance), SafeDivide(A.Z, B, Tolerance), SafeDivide(A.W, B, Tolerance));
		}
		else
		{
			return UE::Math::TVector4<T>(SafeDivide(A.X, B.X, Tolerance), SafeDivide(A.Y, B.Y, Tolerance), SafeDivide(A.Z, B.Z, Tolerance), SafeDivide(A.W, B.W, Tolerance));
		}
	}

	
	/** Returns true if any component of the vector is zero (or nearly zero for float vectors). */
	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TIntPoint<T>& Vector)
	{
		return Vector.X == static_cast<T>(0) || Vector.Y == static_cast<T>(0);
	}

	/** Returns true if any component of the vector is zero (or nearly zero for float vectors). */
	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TIntVector2<T>& Vector)
	{
		return Vector.X == static_cast<T>(0) || Vector.Y == static_cast<T>(0);
	}

	/** Returns true if any component of the vector is zero (or nearly zero for float vectors). */
	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TVector2<T>& Vector, const double Tolerance = 1e-8)
	{
		if constexpr (TIsFloatingPoint<T>::Value)
		{
			return FMath::IsNearlyZero(static_cast<double>(Vector.X), Tolerance)
				|| FMath::IsNearlyZero(static_cast<double>(Vector.Y), Tolerance);
		}
		else
		{
			return Vector.X == static_cast<T>(0) || Vector.Y == static_cast<T>(0);
		}
	}

	/** Returns true if any component of the vector is zero. */
	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TIntVector3<T>& Vector)
	{
		return Vector.X == static_cast<T>(0)
			|| Vector.Y == static_cast<T>(0)
			|| Vector.Z == static_cast<T>(0);
	}

	/** Returns true if any component of the vector is zero. */
	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TVector<T>& Vector, const double Tolerance = 1e-8)
	{
		if constexpr (TIsFloatingPoint<T>::Value)
		{
			return FMath::IsNearlyZero(static_cast<double>(Vector.X), Tolerance)
				|| FMath::IsNearlyZero(static_cast<double>(Vector.Y), Tolerance)
				|| FMath::IsNearlyZero(static_cast<double>(Vector.Z), Tolerance);
		}
		else
		{
			return Vector.X == static_cast<T>(0)
				|| Vector.Y == static_cast<T>(0)
				|| Vector.Z == static_cast<T>(0);
		}
	}

	/** Returns true if any component of the vector is zero. */
	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TIntVector4<T>& Vector)
	{
		return Vector.X == static_cast<T>(0)
			|| Vector.Y == static_cast<T>(0)
			|| Vector.Z == static_cast<T>(0)
			|| Vector.W == static_cast<T>(0);
	}

	template <typename T>
	[[nodiscard]] FORCEINLINE static bool VectorHasZeroAxis(const UE::Math::TVector4<T>& Vector, const double Tolerance = 1e-8)
	{
		if constexpr (TIsFloatingPoint<T>::Value)
		{
			return FMath::IsNearlyZero(static_cast<double>(Vector.X), Tolerance)
				|| FMath::IsNearlyZero(static_cast<double>(Vector.Y), Tolerance)
				|| FMath::IsNearlyZero(static_cast<double>(Vector.Z), Tolerance)
				|| FMath::IsNearlyZero(static_cast<double>(Vector.W), Tolerance);
		}
		else
		{
			return Vector.X == static_cast<T>(0)
				|| Vector.Y == static_cast<T>(0)
				|| Vector.Z == static_cast<T>(0)
				|| Vector.W == static_cast<T>(0);
		}
	}

	
	/** Applies RotatorB after RotatorA and returns the composed rotation. */
	FORCEINLINE static FRotator ComposeRotators(const FRotator& RotatorA, const FRotator& RotatorB)
	{
		const FQuat AQuat = FQuat(RotatorA);
		const FQuat BQuat = FQuat(RotatorB);
		return FRotator(BQuat * AQuat);
	}

	
	/** Rearrange or swap TVector axes according to swizzle rule. */
	template <typename T>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector<T> VectorSwizzleAxes(
		const UE::Math::TVector<T>& Vector,
		EVSAxesSwizzle::Type Swizzle = EVSAxesSwizzle::XYZ)
	{
		UE::Math::TVector<T> Value = Vector;
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
			Value = UE::Math::TVector<T>(Value.Y, Value.Z, Value.X);
			break;

		case EVSAxesSwizzle::ZXY:
			Value = UE::Math::TVector<T>(Value.Z, Value.X, Value.Y);
			break;

		default:;
		}
		return Value;
	}

	/** Rearrange or swap TIntVector3 axes according to swizzle rule. */
	template <typename T>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector3<T> VectorSwizzleAxes(
		const UE::Math::TIntVector3<T>& Vector,
		EVSAxesSwizzle::Type Swizzle = EVSAxesSwizzle::XYZ)
	{
		UE::Math::TIntVector3<T> Value = Vector;
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
			Value = UE::Math::TIntVector3<T>(Value.Y, Value.Z, Value.X);
			break;

		case EVSAxesSwizzle::ZXY:
			Value = UE::Math::TIntVector3<T>(Value.Z, Value.X, Value.Y);
			break;

		default:;
		}
		return Value;
	}

	
	/** Projects rotation onto a plane defined by PlaneNormal. */
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
	 * Returns From with selected axes replaced by To, evaluated in AxesSpace.
	 * @param AxesSpace Reference space used to compare and apply axes.
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

	
	/** Returns From with selected components replaced by To. */
	template<typename T>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector2<T> VectorApplyAxes(
		const UE::Math::TIntVector2<T>& From,
		const UE::Math::TIntVector2<T>& To,
		bool bApplyX = true, bool bApplyY = true)
	{
		UE::Math::TIntVector2<T> AnsVector = From;

		if (bApplyX) AnsVector.X = To.X;
		if (bApplyY) AnsVector.Y = To.Y;

		return AnsVector;
	}

	/** Returns From with selected components replaced by To. */
	template<typename T>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntPoint<T> VectorApplyAxes(
		const UE::Math::TIntPoint<T>& From,
		const UE::Math::TIntPoint<T>& To,
		bool bApplyX = true, bool bApplyY = true)
	{
		UE::Math::TIntPoint<T> AnsVector = From;

		if (bApplyX) AnsVector.X = To.X;
		if (bApplyY) AnsVector.Y = To.Y;

		return AnsVector;
	}
	
	/** Returns From with selected components replaced by To. */
	template<typename T>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector<T> VectorApplyAxes(
		const UE::Math::TVector<T>& From,
		const UE::Math::TVector<T>& To,
		EVSVectorAxes::Type AxesToApply = EVSVectorAxes::Type::XYZ)
	{
		UE::Math::TVector<T> AnsVector = From;

		if (AxesToApply & EVSVectorAxes::X) AnsVector.X = To.X;
		if (AxesToApply & EVSVectorAxes::Y) AnsVector.Y = To.Y;
		if (AxesToApply & EVSVectorAxes::Z) AnsVector.Z = To.Z;

		return AnsVector;
	}
	
	/** Returns From with selected components replaced by To. */
	template<typename T>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector3<T> VectorApplyAxes(
		const UE::Math::TIntVector3<T>& From,
		const UE::Math::TIntVector3<T>& To,
		EVSVectorAxes::Type AxesToApply = EVSVectorAxes::Type::XYZ)
	{
		UE::Math::TIntVector3<T> AnsVector = From;

		if (AxesToApply & EVSVectorAxes::X) AnsVector.X = To.X;
		if (AxesToApply & EVSVectorAxes::Y) AnsVector.Y = To.Y;
		if (AxesToApply & EVSVectorAxes::Z) AnsVector.Z = To.Z;

		return AnsVector;
	}
	
	/**
	 * Make an FVector from source, applying To target axes in specified space.
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
	 * Returns From with selected transform axes replaced by To, evaluated in AxesSpace.
	 * @param AxesSpace Reference space used to compare and apply axes.
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
	 * Interpolates scalar values with optional sub-stepping for large DeltaTime.
	 * @param InterpSpeed Interpolation speed passed to FInterpTo.
	 * @param MaxTimeStep Maximum sub-step size. Set <= 0 to disable sub-stepping.
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
	 * Interpolates rotation per-axis in InterpSpace with optional DeltaTime sub-stepping.
	 * @param InterpSpeed Per-axis interpolation speed (Roll/Pitch/Yaw).
	 * @param MaxTimeStep Maximum sub-step size. Set <= 0 to disable sub-stepping.
	 * @param InterpSpace Reference space used for interpolation.
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

	/** Interpolates FVector2D per-axis with optional DeltaTime sub-stepping. */
	[[nodiscard]] FORCEINLINE static FVector2D VInterpToAdvanced2D(
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
	 * Interpolates vector components in InterpSpace with optional DeltaTime sub-stepping.
	 * @param MaxTimeStep Maximum sub-step size. Set <= 0 to disable sub-stepping.
	 * @param InterpSpace Reference space used for interpolation.
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
	 * Interpolates transform rotation/translation/scale with optional DeltaTime sub-stepping.
	 * @param MaxTimeStep Maximum sub-step size. Set <= 0 to disable sub-stepping.
	 * @param InterpSpace Reference space used for rotation and translation interpolation.
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
	

	/** Clamps TIntPoint with per-axis point min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntPoint<T> ClampVector(
		const UE::Math::TIntPoint<T>& Source,
		const UE::Math::TIntPoint<MinT>& RangeMin,
		const UE::Math::TIntPoint<MaxT>& RangeMax)
	{
		return UE::Math::TIntPoint<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)));
	}

	/** Clamps TIntVector2 with scalar min/max applied to all axes. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector2<T> ClampVector(
		const UE::Math::TIntVector2<T>& Source,
		const MinT& RangeMin,
		const MaxT& RangeMax)
	{
		const UE::Math::TIntPoint<T> Ans = ClampVector(UE::Math::TIntPoint<T>(Source.X, Source.Y), RangeMin, RangeMax);
		return UE::Math::TIntVector2<T>(Ans.X, Ans.Y);
	}

	/** Clamps TIntVector2 with per-axis vector min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector2<T> ClampVector(
		const UE::Math::TIntVector2<T>& Source,
		const UE::Math::TIntVector2<MinT>& RangeMin,
		const UE::Math::TIntVector2<MaxT>& RangeMax)
	{
		return UE::Math::TIntVector2<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)));
	}

	/** Clamps TVector2 with per-axis vector min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector2<T> ClampVector(
		const UE::Math::TVector2<T>& Source,
		const UE::Math::TVector2<MinT>& RangeMin,
		const UE::Math::TVector2<MaxT>& RangeMax)
	{
		return UE::Math::TVector2<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)));
	}

	/** Clamps TIntVector3 with per-axis vector min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector3<T> ClampVector(
		const UE::Math::TIntVector3<T>& Source,
		const UE::Math::TIntVector3<MinT>& RangeMin,
		const UE::Math::TIntVector3<MaxT>& RangeMax)
	{
		return UE::Math::TIntVector3<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)),
			Clamp(static_cast<T>(Source.Z), static_cast<T>(RangeMin.Z), static_cast<T>(RangeMax.Z)));
	}

	/** Clamps TVector with per-axis vector min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector<T> ClampVector(
		const UE::Math::TVector<T>& Source,
		const UE::Math::TVector<MinT>& RangeMin,
		const UE::Math::TVector<MaxT>& RangeMax)
	{
		return UE::Math::TVector<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)),
			Clamp(static_cast<T>(Source.Z), static_cast<T>(RangeMin.Z), static_cast<T>(RangeMax.Z)));
	}

	/** Clamps TIntVector4 with per-axis vector min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TIntVector4<T> ClampVector(
		const UE::Math::TIntVector4<T>& Source,
		const UE::Math::TIntVector4<MinT>& RangeMin,
		const UE::Math::TIntVector4<MaxT>& RangeMax)
	{
		return UE::Math::TIntVector4<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)),
			Clamp(static_cast<T>(Source.Z), static_cast<T>(RangeMin.Z), static_cast<T>(RangeMax.Z)),
			Clamp(static_cast<T>(Source.W), static_cast<T>(RangeMin.W), static_cast<T>(RangeMax.W)));
	}

	/** Clamps TVector4 with per-axis vector min/max. */
	template <typename T, typename MinT, typename MaxT>
	[[nodiscard]] FORCEINLINE static UE::Math::TVector4<T> ClampVector(
		const UE::Math::TVector4<T>& Source,
		const UE::Math::TVector4<MinT>& RangeMin,
		const UE::Math::TVector4<MaxT>& RangeMax)
	{
		return UE::Math::TVector4<T>(
			Clamp(static_cast<T>(Source.X), static_cast<T>(RangeMin.X), static_cast<T>(RangeMax.X)),
			Clamp(static_cast<T>(Source.Y), static_cast<T>(RangeMin.Y), static_cast<T>(RangeMax.Y)),
			Clamp(static_cast<T>(Source.Z), static_cast<T>(RangeMin.Z), static_cast<T>(RangeMax.Z)),
			Clamp(static_cast<T>(Source.W), static_cast<T>(RangeMin.W), static_cast<T>(RangeMax.W)));
	}
	
	/** Clamps world location in ConstrainSpace, then transforms the result back to world space. */
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
	
	/** Clamps direction vector components in ConstrainSpace (no scale), then transforms back. */
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
	
	/** Clamps scale components in ConstrainSpace and returns world-space scale. */
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
	
	/** Clamps Euler angles in ConstrainSpace. If Min > Max, ClampAngle inverse range behavior applies. */
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

	/** Clamps transform rotation, translation, and scale in ConstrainSpace. */
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
	
	/** Computes projected 2D diagonal size along a yaw angle (degrees). */
	[[nodiscard]] FORCEINLINE static double CalcVector2DDiagonalSize(const FVector2D& Vector2D, float AngleYaw)
	{
		return Abs(SafeDivide(Vector2D.X * Vector2D.Y, (Vector2D.X * Sin(UE_DOUBLE_PI / (180.0) * AngleYaw)) + Vector2D.Y * Cos(UE_DOUBLE_PI / (180.0) * AngleYaw)));
	}
	
	/** Computes projected 3D diagonal size along a world-space rotation. */
	[[nodiscard]] FORCEINLINE static double CalcVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation)
	{
		const float SizeXY = CalcVector2DDiagonalSize(FVector2D(Vector.X, Vector.Y), Rotation.Yaw);
		const FVector Direction = Rotation.Vector();
		const float AngleZ = (180.0) / UE_DOUBLE_PI * Acos(SafeDivide(Direction.Z, Direction.Size()));
		return CalcVector2DDiagonalSize(FVector2D(Vector.Z, SizeXY), AngleZ);
	}
};
