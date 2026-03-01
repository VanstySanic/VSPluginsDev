// Copyright Vansty...
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/Math/VSMathTypes.h"
#include "VSMathLibrary.generated.h"

#define VS_MATH_INLINE_ENABLED	(!UE_BUILD_DEBUG)

/**
 * Blueprint function-library wrapper around FVSMath utility methods.
 */
UCLASS()
class VSPLUGINSCORE_API UVSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Safely divides A by B for int32 values. Returns 0 if B is 0. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (int32)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static int32 SafeDivideInt32(int32 A, int32 B);
	
	/** Safely divides A by B for int64 values. Returns 0 if B is 0. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (int64)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static int64 SafeDivideInt64(int64 A, int64 B);

	/** Safely divides A by B for float values. Returns 0.f if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (Float)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static float SafeDivideFloat(float A, float B, double Tolerance = 1e-8);

	/** Safely divides A by B for double values. Returns 0.0 if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (Double)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static double SafeDivideDouble(double A, double B, double Tolerance = 1e-8);
		
	/** Safely divides FIntPoint components. Each component returns 0 when its divisor is 0. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (Int Point)", CompactNodeTitle = "Safe ÷", Keywords = "/", AutoCreateRefTerm = "A, B"))
	static FIntPoint SafeDivideIntPoint(const FIntPoint& A, const FIntPoint& B);
	
	/** Safely divides FVector2D components. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (CompactNodeTitle = "Safe ÷", Keywords = "/", AutoCreateRefTerm = "A, B"))
	static FVector2D SafeDivideVector2D(const FVector2D& A, const FVector2D& B, double Tolerance = 1e-8);

	/** Safely divides FVector components. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (CompactNodeTitle = "Safe ÷", Keywords = "/", AutoCreateRefTerm = "A, B"))
	static FVector SafeDivideVector(const FVector& A, const FVector& B, double Tolerance = 1e-8);

	/** Returns true if any FVector component is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "Vector"))
	static bool VectorHasZeroAxis(const FVector& Vector, double Tolerance = 1e-8);
	
	/** Returns true if any FVector2D component is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "Vector"))
	static bool VectorHasZeroAxis2D(const FVector2D& Vector, double Tolerance = 1e-8);
	
	/** Projects rotation onto a plane defined by PlaneNormal. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (AutoCreateRefTerm = "Rotator, PlaneNormal"))
	static FRotator RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal);

	/** Apply selected axes from another rotator in the given space. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (AutoCreateRefTerm = "From, To, AxesSpace"))
	static FRotator RotatorApplyAxes(
		const FRotator& From,
		const FRotator& To,
		EVSRotatorAxes::Type AxesToApply,
		const FRotator& AxesSpace = FRotator::ZeroRotator);

	/** Apply selected axes of a vector in the given space. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "From, To, AxesSpace"))
	static FVector VectorApplyAxes(
		const FVector& From,
		const FVector& To,
		EVSVectorAxes::Type AxesToApply,
		const FRotator& AxesSpace);

	/** Apply selected axes of a transform in the given space. */
	UFUNCTION(BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, AxesSpace"))
	static FTransform TransformApplyAxes(
		const FTransform& From,
		const FTransform& To,
		const FVSTransformAxes& AxesToApply,
		const FRotator& AxesSpace);

	/** Sub-stepped interpolation for float values. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static float FloatInterpToAdvanced(
		float From,
		float To,
		float DeltaTime,
		float InterpSpeed = 10.f,
		float MaxTimeStep = 0.016667f);

	/** Sub-stepped interpolation for double values. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static double DoubleInterpToAdvanced(
		double From,
		double To,
		double DeltaTime,
		double InterpSpeed = 10.f,
		double MaxTimeStep = 0.016667f);

	/** Sub-stepped per-axis rotator interpolation in InterpSpace. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation", meta = (AutoCreateRefTerm = "From, To, InterpSpace"))
	static FRotator RotatorInterpToAdvanced(
		const FRotator& From,
		const FRotator& To,
		float DeltaTime,
		const FRotator& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator);

	/** Sub-stepped per-axis FVector2D interpolation. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation", meta = (AutoCreateRefTerm = "From, To, InterpSpace"))
	static FVector2D VectorInterpToAdvanced2D(
		const FVector2D& From,
		const FVector2D& To,
		float DeltaTime,
		const FVector2D& InterpSpeed,
		float MaxTimeStep = 0.016667f);

	/** Sub-stepped per-axis FVector interpolation in InterpSpace. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation", meta = (AutoCreateRefTerm = "From, To, InterpSpace"))
	static FVector VectorInterpToAdvanced(
		const FVector& From,
		const FVector& To,
		float DeltaTime,
		const FVector& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator);

	/** Sub-stepped transform interpolation (rotation/translation/scale). */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation", meta = (AutoCreateRefTerm = "From, To, InterpSpace"))
	static FTransform TransformInterpToAdvanced(
		const FTransform& From,
		const FTransform& To,
		float DeltaTime,
		const FTransform& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator);

	/** Clamps FVector2D component-wise between RangeMin and RangeMax. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector2D", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax"))
	static FVector2D ClampVector2D(
		const FVector2D& Source,
		const FVector2D& RangeMin,
		const FVector2D& RangeMax);

	/** Clamps world-space location in ClampSpace. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ClampSpace"))
	static FVector ClampVectorLocation(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FTransform& ClampSpace);

	/** Clamps direction-vector components in ClampSpace. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ClampSpace"))
	static FVector ClampVectorDirection(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FRotator& ClampSpace);

	/** Clamps scale components in ClampSpace. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ClampSpace"))
	static FVector ClampVectorScale(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FTransform& ClampSpace);

	/** Clamps rotator axes in ClampSpace (supports inverse ClampAngle ranges). */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ClampSpace"))
	static FRotator ClampRotator(
		const FRotator& Source,
		const FRotator& RangeMin,
		const FRotator& RangeMax,
		const FRotator& ClampSpace);

	/** Clamps rotation, translation, and scale in ClampSpace. */
	UFUNCTION(BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ClampSpace"))
	static FTransform ClampTransform(
		const FTransform& Source,
		const FTransform& RangeMin,
		const FTransform& RangeMax,
		const FTransform& ClampSpace);

	/** Computes projected 2D diagonal size along a yaw angle (degrees). */
	UFUNCTION(BlueprintPure, Category = "Math|Vector2D", meta = (AutoCreateRefTerm = "Vector2D"))
	static float CalcVectorDiagonalSize2D(const FVector2D& Vector2D, float AngleYaw);

	/** Computes projected 3D diagonal size along a world-space rotation. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (AutoCreateRefTerm = "Vector"))
	static float CalcVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation);
};
