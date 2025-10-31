// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/VSMathTypes.h"
#include "VSMathLibrary.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintThreadSafe))
class VSPLUGINSCORE_API UVSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** 
	 * Get number of decimal places of a float after trimming trailing zeros.
	 * Handles artifacts like 0.29999999998f / 0.1000000001f correctly.
	 */
	UFUNCTION(BlueprintPure, Category = "Math|Number")
	static int32 GetDecimalPlacesTrimmedFloat(float Value, int32 MaxDecimals = 12);

	/** Check if a vector has any axis nearly zero within tolerance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector"))
	static bool VectorHasZeroAxis(const FVector& Vector, const float Tolerance = 0.00000001);

	/** Safely divide VectorA by VectorB (per component), avoiding divide-by-zero. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector"))
	static FVector VectorSafeDivide(const FVector& VectorA, const FVector& VectorB);

	/** Rearrange or swap vector axes according to swizzle rule. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector"))
	static FVector VectorSwizzleAxes(const FVector& Vector, EVSAxesSwizzle::Type Swizzle = EVSAxesSwizzle::XZY);

	/** Project a rotator orientation onto a plane defined by a normal. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Rotator, PlaneNormal"))
	static FRotator RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal);

	/** Local to world. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector, Transform"))
	static FVector TransformVectorWithScale(const FVector& Vector, const FTransform& Transform);

	/** World to local. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector, Transform"))
	static FVector InverseTransformVectorWithScale(const FVector& Vector, const FTransform& Transform);
	
	/**
	 * Make a rotator from source, applying target rotator axes in specified space.
	 * @param AxesSpace Axes will be applied in that space.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, AxesSpace"))
	static FRotator RotatorApplyAxes(const FRotator& From, const FRotator& To, EVSRotatorAxes::Type AxesToApply = EVSRotatorAxes::Type::RollPitchYaw, const FRotator& AxesSpace = FRotator::ZeroRotator);
	
	/**
	 * Make a vector from source, applying To target axes in specified space.
	 * @param AxesSpace Axes will be applied in that space.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, AxesSpace"))
	static FVector VectorApplyAxes(const FVector& From, const FVector& To, EVSVectorAxes::Type AxesToApply = EVSVectorAxes::Type::XYZ, const FRotator& AxesSpace = FRotator::ZeroRotator);
	
	/**
	 * Make a transform from source, applying target transform axes in specified space.
	 * @param AxesSpace Axes will be applied in that space.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, AxesSpace"))
	static FTransform TransformApplyAxes(const FTransform& From, const FTransform& To, const FVSTransformAxes& AxesToApply = FVSTransformAxes(), const FRotator& AxesSpace = FRotator::ZeroRotator);
	
	
	/**
	 * Interpolate a float with optional time sub-stepping.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform")
	static float FloatInterpTo(const float From, const float To, float DeltaTime, const float InterpSpeed, float MaxTimeStep = 0.016667f);

	/**
	 * Interpolate a double with optional time sub-stepping.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform")
	static double DoubleInterpTo(const double From, const double To, float DeltaTime, const float InterpSpeed, float MaxTimeStep = 0.016667f);
	
	/**
	 * Interpolate a rotator in specified space with optional time sub-stepping.
	 * @param InterpSpeed Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, InterpSpeed, InterpSpace"))
	static FRotator RotatorInterpTo(const FRotator& From, const FRotator& To, float DeltaTime, const FRotator& InterpSpeed, bool bConstantSpeed = false, float MaxTimeStep = 0.016667f, const FRotator& InterpSpace = FRotator::ZeroRotator);
	
	/**
	 * Interpolate a vector in specified space with optional time sub-stepping.
	 * @param InterpSpace Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, InterpSpeed, InterpSpace"))
	static FVector VectorInterpTo(const FVector& From, const FVector& To, float DeltaTime, const FVector& InterpSpeed, bool bConstantSpeed = false, float MaxTimeStep = 0.016667f, const FRotator& InterpSpace = FRotator::ZeroRotator);
	
	/**
	 * Interpolate a transform in specified space with optional time sub-stepping.
	 * @param InterpSpace Interpolation will be executed in that space.
	 * @param MaxTimeStep Max time step used when sub-stepping interpolation. If > 0.f, it will handle fluctuating frame rates well (though this comes at a cost).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "From, To, InterpSpeed, InterpSpace"))
	static FTransform TransformInterpTo(const FTransform& From, const FTransform& To, float DeltaTime, const FTransform& InterpSpeed, bool bConstantSpeed = false, float MaxTimeStep = 0.016667f, const FRotator& InterpSpace = FRotator::ZeroRotator);
	
	
	/** Clamp vector translation. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ConstrainSpace"))
	static FVector ClampVectorTranslation(const FVector& Source, const FVector& RangeMin, const FVector& RangeMax, bool bTranslationAsLocation, const FTransform& ConstrainSpace = FTransform());
	
	/** Clamp vector scale. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ConstrainSpace"))
	static FVector ClampVectorScale(const FVector& Source, const FVector& RangeMin, const FVector& RangeMax, const FTransform& ConstrainSpace);
	
	/** Clamp rotation angle. Range min larger than range max indicates the inversed area. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ConstrainSpace"))
	static FRotator ClampRotation(const FRotator& Source, const FRotator& RangeMin, const FRotator& RangeMax, const FRotator& ConstrainSpace = FRotator::ZeroRotator);
	
	/** Clamp transform for all axes. Rotation range min larger than range max means inverse. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Source, RangeMin, RangeMax, ConstrainSpace"))
	static FTransform ClampTransform(const FTransform& Source, const FTransform& RangeMin, const FTransform& RangeMax, bool bTranslationAsLocation, const FTransform& ConstrainSpace = FTransform());

	/** Get diagonal size of a 2D vector along specified yaw angle. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector2D"))
	static float GetVectorDiagonalSize2D(const FVector2D& Vector2D, float AngleYaw);

	/** Get diagonal size of a 3D vector along specified rotation. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Transform", meta = (AutoCreateRefTerm = "Vector, Rotation"))
	static float GetVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation);
};
