// Copyright VanstySanic. All Rights Reserved.

#include "Features/Composition/VSChrMovFeature_InputAxesProcessor.h"
#include "GameFramework/Character.h"
#include "Libraries/VSMathLibrary.h"

UVSChrMovFeature_InputAxesProcessor::UVSChrMovFeature_InputAxesProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovFeature_InputAxesProcessor::ProcessInputAxes(const FVector& Axes, EVSAxesSwizzle::Type Swizzle)
{
	if (Axes.IsNearlyZero()) return;
	const FVector& MovementInput = GetDesiredMovementInput(Axes, Swizzle);
	AddMovementInput(MovementInput);
}

void UVSChrMovFeature_InputAxesProcessor::AddMovementInput(const FVector& MovementInput)
{
	if (MovementInput.IsNearlyZero() || !GetCharacter()) return;
	GetCharacter()->AddMovementInput(MovementInput.GetSafeNormal(), MovementInput.Size());
}

FVector UVSChrMovFeature_InputAxesProcessor::GetDesiredMovementInput_Implementation(const FVector& Axes, EVSAxesSwizzle::Type Swizzle)
{
	const FVector& SwizzledAxes = UVSMathLibrary::VectorSwizzleAxes(Axes, Swizzle);
	return GetMovementInputForCommonMovement2D(FVector2D(SwizzledAxes.X, SwizzledAxes.Y), false);
}

FVector UVSChrMovFeature_InputAxesProcessor::GetMovementInputForCommonMovement2D(const FVector2D& Axes2D, bool bSwizzleAxesXY)
{
	const FVector& ForwardVector = FVector::VectorPlaneProject(GetControlRotation().Quaternion().GetForwardVector(), GetUpDirection()).GetSafeNormal();
	const FVector& RightVector = FVector::VectorPlaneProject(GetControlRotation().Quaternion().GetRightVector(), GetUpDirection()).GetSafeNormal();

	const FVector2D& SwizzledAxes = bSwizzleAxesXY ? FVector2D(Axes2D.Y, Axes2D.X) : Axes2D;
	const FVector& MovementInput = ForwardVector * SwizzledAxes.X + RightVector * SwizzledAxes.Y;
	return MovementInput;
}
