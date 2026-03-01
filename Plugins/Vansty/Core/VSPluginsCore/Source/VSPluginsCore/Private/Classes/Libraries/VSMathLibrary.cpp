// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSMathLibrary.h"

#include "Kismet/KismetMathLibrary.h"
#include "Types/Math/VSMath.h"
#include "Types/Math/VSMathTypes.h"

UVSMathLibrary::UVSMathLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

int32 UVSMathLibrary::SafeDivideInt32(int32 A, int32 B)
{
	return FVSMath::SafeDivide(A, B);
}

int64 UVSMathLibrary::SafeDivideInt64(int64 A, int64 B)
{
	return FVSMath::SafeDivide(A, B);
}

float UVSMathLibrary::SafeDivideFloat(float A, float B, double Tolerance)
{
	return FVSMath::SafeDivide(A, B, Tolerance);
}

double UVSMathLibrary::SafeDivideDouble(double A, double B, double Tolerance)
{
	return FVSMath::SafeDivide(A, B, Tolerance);
}

FIntPoint UVSMathLibrary::SafeDivideIntPoint(const FIntPoint& A, const FIntPoint& B)
{
	return FVSMath::SafeDivide(A, B);
}

FVector2D UVSMathLibrary::SafeDivideVector2D(const FVector2D& A, const FVector2D& B, double Tolerance)
{
	return FVSMath::SafeDivide(A, B, Tolerance);
}

FVector UVSMathLibrary::SafeDivideVector(const FVector& A, const FVector& B, double Tolerance)
{
	return FVSMath::SafeDivide(A, B, Tolerance);
}

bool UVSMathLibrary::VectorHasZeroAxis(const FVector& Vector, double Tolerance)
{
	return FVSMath::VectorHasZeroAxis(Vector, Tolerance);
}

bool UVSMathLibrary::VectorHasZeroAxis2D(const FVector2D& Vector, double Tolerance)
{
	return FVSMath::VectorHasZeroAxis(Vector, Tolerance);
}

FRotator UVSMathLibrary::RotatorProjectOntoPlane(const FRotator& Rotator, const FVector& PlaneNormal)
{
	return FVSMath::RotatorProjectOntoPlane(Rotator, PlaneNormal);
}

FRotator UVSMathLibrary::RotatorApplyAxes(
	const FRotator& From,
	const FRotator& To,
	EVSRotatorAxes::Type AxesToApply,
	const FRotator& AxesSpace)
{
	return FVSMath::RotatorApplyAxes(From, To, AxesToApply, AxesSpace);
}

FVector UVSMathLibrary::VectorApplyAxes(
	const FVector& From,
	const FVector& To,
	EVSVectorAxes::Type AxesToApply,
	const FRotator& AxesSpace)
{
	return FVSMath::VectorApplyAxes(From, To, AxesToApply, AxesSpace);
}

FTransform UVSMathLibrary::TransformApplyAxes(
	const FTransform& From,
	const FTransform& To,
	const FVSTransformAxes& AxesToApply,
	const FRotator& AxesSpace)
{
	return FVSMath::TransformApplyAxes(From, To, AxesToApply, AxesSpace);
}

float UVSMathLibrary::FloatInterpToAdvanced(
	float From,
	float To,
	float DeltaTime,
	float InterpSpeed,
	float MaxTimeStep)
{
	return FVSMath::FInterpToAdvanced<float>(From, To, DeltaTime, InterpSpeed, MaxTimeStep);
}

double UVSMathLibrary::DoubleInterpToAdvanced(double From, double To, double DeltaTime, double InterpSpeed, double MaxTimeStep)
{
	return FVSMath::FInterpToAdvanced<double>(From, To, DeltaTime, InterpSpeed, MaxTimeStep);
}

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

FVector2D UVSMathLibrary::VectorInterpToAdvanced2D(
	const FVector2D& From,
	const FVector2D& To,
	float DeltaTime,
	const FVector2D& InterpSpeed,
	float MaxTimeStep)
{
	return FVSMath::VInterpToAdvanced2D(From, To, DeltaTime, InterpSpeed, MaxTimeStep);
}

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

FVector2D UVSMathLibrary::ClampVector2D(
	const FVector2D& Source,
	const FVector2D& RangeMin,
	const FVector2D& RangeMax)
{
	return FVSMath::ClampVector(Source, RangeMin, RangeMax);
}

FVector UVSMathLibrary::ClampVectorLocation(
	const FVector& Source,
	const FVector& RangeMin,
	const FVector& RangeMax,
	const FTransform& ClampSpace)
{
	return FVSMath::ClampVectorLocation(Source, RangeMin, RangeMax, ClampSpace);
}

FVector UVSMathLibrary::ClampVectorDirection(
	const FVector& Source,
	const FVector& RangeMin,
	const FVector& RangeMax,
	const FRotator& ClampSpace)
{
	return FVSMath::ClampVectorDirection(Source, RangeMin, RangeMax, ClampSpace);
}

FVector UVSMathLibrary::ClampVectorScale(
	const FVector& Source,
	const FVector& RangeMin,
	const FVector& RangeMax,
	const FTransform& ClampSpace)
{
	return FVSMath::ClampVectorScale(Source, RangeMin, RangeMax, ClampSpace);
}

FRotator UVSMathLibrary::ClampRotator(
	const FRotator& Source,
	const FRotator& RangeMin,
	const FRotator& RangeMax,
	const FRotator& ClampSpace)
{
	return FVSMath::ClampRotator(Source, RangeMin, RangeMax, ClampSpace);
}

FTransform UVSMathLibrary::ClampTransform(
	const FTransform& Source,
	const FTransform& RangeMin,
	const FTransform& RangeMax,
	const FTransform& ClampSpace)
{
	return FVSMath::ClampTransform(Source, RangeMin, RangeMax, ClampSpace);
}

float UVSMathLibrary::CalcVectorDiagonalSize2D(const FVector2D& Vector2D, float AngleYaw)
{
	return static_cast<float>(FVSMath::CalcVector2DDiagonalSize(Vector2D, AngleYaw));
}

float UVSMathLibrary::CalcVectorDiagonalSize(const FVector& Vector, const FRotator& Rotation)
{
	return static_cast<float>(FVSMath::CalcVectorDiagonalSize(Vector, Rotation));
}

