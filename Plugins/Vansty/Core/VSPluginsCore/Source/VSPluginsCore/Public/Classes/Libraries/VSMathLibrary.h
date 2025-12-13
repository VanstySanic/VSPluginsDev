// Copyright Vansty...
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/VSMathTypes.h"
#include "VSMathLibrary.generated.h"

#define VS_MATH_INLINE_ENABLED	(!UE_BUILD_DEBUG)

/**
 * UVSMathLibrary
 *
 * Blueprint-accessible wrapper for FVSMath helper functions.
 * Provides a set of math utilities for safe division, interpolation,
 * clamping and geometric queries.
 */
UCLASS()
class VSPLUGINSCORE_API UVSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Safe int64 divide (A/B). Returns 0 if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (int32)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static int32 SafeDivideInt32(int32 A, int32 B);
	
	/** Safe int64 divide (A/B). Returns 0 if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (int64)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static int64 SafeDivideInt64(int64 A, int64 B);
	
	/** Safe FIntPoint divide (component-wise). Returns 0 if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (Int Point)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static FIntPoint SafeDivideIntPoint(const FIntPoint& A, const FIntPoint& B);
	
	/** Safe float divide (A/B). Returns 0 if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (Float)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static float SafeDivideFloat(float A, float B);

	/** Safe double divide (A/B). Returns 0 if B is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (DisplayName = "Safe Divide (Double)", CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static double SafeDivideDouble(double A, double B);
	
	/** Safe Vector2D divide (component-wise). */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static FVector2D SafeDivideVector2D(const FVector2D& A, const FVector2D& B);

	/** Safe Vector divide (component-wise). */
	UFUNCTION(BlueprintPure, Category = "Math|Operators", meta = (CompactNodeTitle = "Safe ÷", Keywords = "/"))
	static FVector SafeDivideVector(const FVector& A, const FVector& B);

	/** Check if any axis of a vector is nearly zero. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static bool VectorHasZeroAxis(const FVector& Vector, double Tolerance = 1e-8);

	/** Project a rotator onto a plane normal. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator")
	static FRotator RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal);

	/** Apply selected axes from another rotator in the given space. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator")
	static FRotator RotatorApplyAxes(
		const FRotator& From,
		const FRotator& To,
		EVSRotatorAxes::Type AxesToApply,
		const FRotator& AxesSpace = FRotator::ZeroRotator);

	/** Apply selected axes of a vector in the given space. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static FVector VectorApplyAxes(
		const FVector& From,
		const FVector& To,
		EVSVectorAxes::Type AxesToApply,
		const FRotator& AxesSpace);

	/** Apply selected axes of a transform in the given space. */
	UFUNCTION(BlueprintPure, Category = "Math|Transform")
	static FTransform TransformApplyAxes(
		const FTransform& From,
		const FTransform& To,
		const FVSTransformAxes& AxesToApply,
		const FRotator& AxesSpace);

	/** Advanced float interpolation (sub-stepped). */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static float FloatInterpToAdvanced(
		float From,
		float To,
		float DeltaTime,
		float InterpSpeed = 10.f,
		float MaxTimeStep = 0.016667f);

	/** Advanced float interpolation (sub-stepped). */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static float DoubleInterpToAdvanced(
		double From,
		double To,
		double DeltaTime,
		double InterpSpeed = 10.f,
		double MaxTimeStep = 0.016667f);

	/** Advanced rotator interpolation (sub-stepped, space aware). */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static FRotator RotatorInterpToAdvanced(
		const FRotator& From,
		const FRotator& To,
		float DeltaTime,
		const FRotator& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator);

	/** Advanced Vector2D interpolation. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static FVector2D Vector2DInterpToAdvanced(
		const FVector2D& From,
		const FVector2D& To,
		float DeltaTime,
		const FVector2D& InterpSpeed,
		float MaxTimeStep = 0.016667f);

	/** Advanced vector interpolation (space aware). */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static FVector VectorInterpToAdvanced(
		const FVector& From,
		const FVector& To,
		float DeltaTime,
		const FVector& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator);

	/** Advanced transform interpolation. */
	UFUNCTION(BlueprintPure, Category = "Math|Interpolation")
	static FTransform TransformInterpToAdvanced(
		const FTransform& From,
		const FTransform& To,
		float DeltaTime,
		const FTransform& InterpSpeed,
		float MaxTimeStep = 0.016667f,
		const FRotator& InterpSpace = FRotator::ZeroRotator);

	/** Clamp a Vector2D. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector2D")
	static FVector2D ClampVector2D(
		const FVector2D& Source,
		const FVector2D& RangeMin,
		const FVector2D& RangeMax);

	/** Clamp world location in space. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static FVector ClampVectorLocation(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FTransform& ConstrainSpace);

	/** Clamp direction vector in space. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static FVector ClampVectorDirection(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FRotator& ConstrainSpace);

	/** Clamp scale in space. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static FVector ClampVectorScale(
		const FVector& Source,
		const FVector& RangeMin,
		const FVector& RangeMax,
		const FTransform& ConstrainSpace);

	/** Clamp a rotator (inverse range allowed). */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator")
	static FRotator ClampRotator(
		const FRotator& Source,
		const FRotator& RangeMin,
		const FRotator& RangeMax,
		const FRotator& ConstrainSpace);

	/** Clamp a full transform. */
	UFUNCTION(BlueprintPure, Category = "Math|Transform")
	static FTransform ClampTransform(
		const FTransform& Source,
		const FTransform& RangeMin,
		const FTransform& RangeMax,
		const FTransform& ConstrainSpace);

	/** Diagonal size of 2D vector at yaw angle. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector2D")
	static float CalcVector2DDiagonalSize( const FVector2D& Vector2D, float AngleYaw);

	/** Diagonal size of 3D vector at rotation. */
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static float CalcVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation);
};
