// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSMathLibrary.h"
#include "Types/VSMath.h"

#if VS_MATH_INLINE_ENABLED
	#define VS_MATH_FORCEINLINE		FORCEINLINE_DEBUGGABLE
	#define VS_MATH_INLINE			inline
#else
	#define KISMET_MATH_FORCEINLINE		// nothing
	#define KISMET_MATH_INLINE			// nothing
#endif

UVSMathLibrary::UVSMathLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

VS_MATH_FORCEINLINE
int32 UVSMathLibrary::SafeDivideInt(int32 A, int32 B)
{
	return FVSMath::SafeDivide<int32>(A, B);
}

VS_MATH_FORCEINLINE
FIntPoint UVSMathLibrary::SafeDivideIntPoint(const FIntPoint& A, const FIntPoint& B)
{
	return FVSMath::SafeDivide<FIntPoint>(A, B);
}

VS_MATH_FORCEINLINE
float UVSMathLibrary::SafeDivideFloat(float A, float B)
{
	return FVSMath::SafeDivide<float>(A, B);
}

VS_MATH_FORCEINLINE
double UVSMathLibrary::SafeDivideDouble(double A, double B)
{
	return FVSMath::SafeDivide<double>(A, B);
}

VS_MATH_FORCEINLINE
FVector2D UVSMathLibrary::SafeDivideVector2D(const FVector2D& A, const FVector2D& B)
{
	return FVSMath::SafeDivide<FVector2D>(A, B);
}

VS_MATH_FORCEINLINE
FVector UVSMathLibrary::SafeDivideVector(const FVector& A, const FVector& B)
{
	return FVSMath::SafeDivide<FVector>(A, B);
}

VS_MATH_FORCEINLINE
bool UVSMathLibrary::VectorHasZeroAxis(const FVector& Vector, double Tolerance)
{
	return FVSMath::VectorHasZeroAxis(Vector, Tolerance);
}

VS_MATH_FORCEINLINE
FRotator UVSMathLibrary::RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal)
{
	return FVSMath::RotatorProjectOntoPlane(Rotator, PlaneNormal);
}

VS_MATH_FORCEINLINE
FRotator UVSMathLibrary::RotatorApplyAxes(
	const FRotator& From,
	const FRotator& To,
	EVSRotatorAxes::Type AxesToApply,
	const FRotator& AxesSpace)
{
	return FVSMath::RotatorApplyAxes(From, To, AxesToApply, AxesSpace);
}

VS_MATH_FORCEINLINE
FVector UVSMathLibrary::VectorApplyAxes(
	const FVector& From,
	const FVector& To,
	EVSVectorAxes::Type AxesToApply,
	const FRotator& AxesSpace)
{
	return FVSMath::VectorApplyAxes(From, To, AxesToApply, AxesSpace);
}

VS_MATH_FORCEINLINE
FTransform UVSMathLibrary::TransformApplyAxes(
	const FTransform& From,
	const FTransform& To,
	const FVSTransformAxes& AxesToApply,
	const FRotator& AxesSpace)
{
	return FVSMath::TransformApplyAxes(From, To, AxesToApply, AxesSpace);
}

VS_MATH_FORCEINLINE
float UVSMathLibrary::FloatInterpToAdvanced(
	float From,
	float To,
	float DeltaTime,
	float InterpSpeed,
	float MaxTimeStep)
{
	return FVSMath::FInterpToAdvanced<float>(From, To, DeltaTime, InterpSpeed, MaxTimeStep);
}

VS_MATH_FORCEINLINE
float UVSMathLibrary::DoubleInterpToAdvanced(double From, double To, double DeltaTime, double InterpSpeed, double MaxTimeStep)
{
	return FVSMath::FInterpToAdvanced<double>(From, To, DeltaTime, InterpSpeed, MaxTimeStep);
}

VS_MATH_FORCEINLINE
FRotator UVSMathLibrary::RotatorInterpToAdvanced(
	const FRotator& From,
	const FRotator& To,
	float DeltaTime,
	const FRotator& InterpSpeed,
	float MaxTimeStep,
	const FRotator& InterpSpace)
{
	return FVSMath::RInterpToAdvanced(From, To, DeltaTime, InterpSpeed, MaxTimeStep, InterpSpace);
}

VS_MATH_FORCEINLINE
FVector2D UVSMathLibrary::Vector2DInterpToAdvanced(
	const FVector2D& From,
	const FVector2D& To,
	float DeltaTime,
	const FVector2D& InterpSpeed,
	float MaxTimeStep)
{
	return FVSMath::VInterpTo2DAdvanced(From, To, DeltaTime, InterpSpeed, MaxTimeStep);
}

VS_MATH_FORCEINLINE
FVector UVSMathLibrary::VectorInterpToAdvanced(
	const FVector& From,
	const FVector& To,
	float DeltaTime,
	const FVector& InterpSpeed,
	float MaxTimeStep,
	const FRotator& InterpSpace)
{
	return FVSMath::VInterpToAdvanced(From, To, DeltaTime, InterpSpeed, MaxTimeStep, InterpSpace);
}

VS_MATH_FORCEINLINE
FTransform UVSMathLibrary::TransformInterpToAdvanced(
	const FTransform& From,
	const FTransform& To,
	float DeltaTime,
	const FTransform& InterpSpeed,
	float MaxTimeStep,
	const FRotator& InterpSpace)
{
	return FVSMath::TransformInterpTo(From, To, DeltaTime, InterpSpeed, MaxTimeStep, InterpSpace);
}

VS_MATH_FORCEINLINE
FVector2D UVSMathLibrary::ClampVector2D(
	const FVector2D& Source,
	const FVector2D& RangeMin,
	const FVector2D& RangeMax)
{
	return FVSMath::ClampVector2D(Source, RangeMin, RangeMax);
}

VS_MATH_FORCEINLINE
FVector UVSMathLibrary::ClampVectorLocation(
	const FVector& Source,
	const FVector& RangeMin,
	const FVector& RangeMax,
	const FTransform& ConstrainSpace)
{
	return FVSMath::ClampVectorLocation(Source, RangeMin, RangeMax, ConstrainSpace);
}

VS_MATH_FORCEINLINE
FVector UVSMathLibrary::ClampVectorDirection(
	const FVector& Source,
	const FVector& RangeMin,
	const FVector& RangeMax,
	const FRotator& ConstrainSpace)
{
	return FVSMath::ClampVectorDirection(Source, RangeMin, RangeMax, ConstrainSpace);
}

VS_MATH_FORCEINLINE
FVector UVSMathLibrary::ClampVectorScale(
	const FVector& Source,
	const FVector& RangeMin,
	const FVector& RangeMax,
	const FTransform& ConstrainSpace)
{
	return FVSMath::ClampVectorScale(Source, RangeMin, RangeMax, ConstrainSpace);
}

VS_MATH_FORCEINLINE
FRotator UVSMathLibrary::ClampRotator(
	const FRotator& Source,
	const FRotator& RangeMin,
	const FRotator& RangeMax,
	const FRotator& ConstrainSpace)
{
	return FVSMath::ClampRotator(Source, RangeMin, RangeMax, ConstrainSpace);
}

VS_MATH_FORCEINLINE
FTransform UVSMathLibrary::ClampTransform(
	const FTransform& Source,
	const FTransform& RangeMin,
	const FTransform& RangeMax,
	const FTransform& ConstrainSpace)
{
	return FVSMath::ClampTransform(Source, RangeMin, RangeMax, ConstrainSpace);
}

VS_MATH_FORCEINLINE
float UVSMathLibrary::CalcVector2DDiagonalSize(const FVector2D& Vector2D, float AngleYaw)
{
	return static_cast<float>(FVSMath::CalcVector2DDiagonalSize(Vector2D, AngleYaw));
}

VS_MATH_FORCEINLINE
float UVSMathLibrary::CalcVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation)
{
	return static_cast<float>(FVSMath::CalcVectorDiagonalSize(Vector, Rotation));
}

