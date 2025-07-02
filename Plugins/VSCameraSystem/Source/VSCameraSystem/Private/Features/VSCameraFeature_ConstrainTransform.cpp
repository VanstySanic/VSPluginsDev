// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_ConstrainTransform.h"
#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/VSMathLibrary.h"

UVSCameraFeature_ConstrainTransform::UVSCameraFeature_ConstrainTransform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraFeature_ConstrainTransform::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	if (!ControllerPrivate)
	{
		ControllerPrivate = UGameplayStatics::GetPlayerController(this, DefaultPlayerIndex);
	}
	if (!ControllerPrivate)
	{
		if (APawn* Pawn = Cast<APawn>(GetCameraComponent()->GetOwner()))
		{
			ControllerPrivate = Pawn->GetController();
		}
	}
	CatchUpMovement();
}

void UVSCameraFeature_ConstrainTransform::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);

	const FTransform& ConstrainSpace = GetCameraRelatedTransformByType(TransformModifySettings.ModifySpaceType, FTransform(TransformLagSettings.CustomLagSpace));
	const FTransform& LagSpace = GetCameraRelatedTransformByType(TransformLagSettings.LagSpaceType, FTransform(TransformLagSettings.CustomLagSpace));
	const FTransform& CameraTransformWS = GetCameraViewData()->CameraTransform;
	const FTransform& ConstrainedTransformWS = UVSMathLibrary::ClampTransform(CameraTransformWS, ConstrainTransformMin, ConstrainTransformMax, true, ConstrainSpace);
	const FTransform& LaggedTransformWS = UVSMathLibrary::TransformInterpTo(LastUpdateTransform, ConstrainedTransformWS, DeltaTime, TransformLagSettings.LagSpeedTransform, false, TransformLagSettings.MaxLagTimeSubStepping, LagSpace.Rotator());
	const FTransform& AxesedTransformWS = UVSMathLibrary::TransformApplyAxes(CameraTransformWS, LaggedTransformWS, TransformModifySettings.AxesToModify, ConstrainSpace.Rotator());
	GetCameraViewData()->CameraTransform = AxesedTransformWS;
	LastUpdateTransform = AxesedTransformWS;

	if (bConstrainControlRotation && ControllerPrivate)
	{
		const FRotator& ConstrainedControlRotationWS = UVSMathLibrary::ClampRotation(ControllerPrivate->GetControlRotation(), ConstrainTransformMin.Rotator(), ConstrainTransformMax.Rotator(), ConstrainSpace.Rotator());
		const FRotator& AxesedControlRotationWS = UVSMathLibrary::RotatorApplyAxes(ControllerPrivate->GetControlRotation(), ConstrainedControlRotationWS, TransformModifySettings.AxesToModify.RotatorAxes, ConstrainSpace.Rotator());
		ControllerPrivate->SetControlRotation(AxesedControlRotationWS);
	}
}

void UVSCameraFeature_ConstrainTransform::CatchUpMovement()
{
	const FTransform& ConstrainSpace = GetCameraRelatedTransformByType(TransformModifySettings.ModifySpaceType, FTransform(TransformLagSettings.CustomLagSpace));
	const FTransform& CameraTransformWS = GetCameraViewData()->CameraTransform;
	const FTransform& ConstrainedTransformWS = UVSMathLibrary::ClampTransform(CameraTransformWS, ConstrainTransformMin, ConstrainTransformMax, true, ConstrainSpace);
	const FTransform& AxesedTransformWS = UVSMathLibrary::TransformApplyAxes(CameraTransformWS, ConstrainedTransformWS, TransformModifySettings.AxesToModify, ConstrainSpace.Rotator());
	GetCameraViewData()->CameraTransform = AxesedTransformWS;
	LastUpdateTransform = AxesedTransformWS;
	if (bConstrainControlRotation && ControllerPrivate)
	{
		const FRotator& ConstrainedControlRotationWS = UVSMathLibrary::ClampRotation(ControllerPrivate->GetControlRotation(), ConstrainTransformMin.Rotator(), ConstrainTransformMax.Rotator(), ConstrainSpace.Rotator());
		const FRotator& AxesedControlRotationWS = UVSMathLibrary::RotatorApplyAxes(ControllerPrivate->GetControlRotation(), ConstrainedControlRotationWS, TransformModifySettings.AxesToModify.RotatorAxes, ConstrainSpace.Rotator());
		ControllerPrivate->SetControlRotation(AxesedControlRotationWS);
	}
}
