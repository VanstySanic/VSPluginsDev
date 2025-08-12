// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_ModifyTransform.h"
#include "VSCameraViewData.h"
#include "Libraries/VSMathLibrary.h"

UVSCameraFeature_ModifyTransform::UVSCameraFeature_ModifyTransform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraFeature_ModifyTransform::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	CatchUpMovement();
}

void UVSCameraFeature_ModifyTransform::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);

	const FRotator& ProcessSpace = GetCameraRelatedRotationByType(TransformModifySettings.ModifySpaceType, TransformModifySettings.CustomModifySpace.Rotator());
	const FRotator& LagSpace = GetCameraRelatedRotationByType(TransformLagSettings.LagSpaceType, TransformLagSettings.CustomLagSpace);

	const FTransform& CameraTransformWS = GetCameraViewData()->CameraTransform;
	const FTransform& TargetTransformWS = GetCameraRelatedTransformByType(ModifyTransformType, CustomTransform);
	const FTransform& LaggedTransformWS = UVSMathLibrary::TransformInterpTo(LastUpdateTransform, TargetTransformWS, DeltaTime, TransformLagSettings.LagSpeedTransform, false, TransformLagSettings.MaxLagTimeSubStepping, LagSpace);
	const FTransform& AxesedTransformWS = UVSMathLibrary::TransformApplyAxes(CameraTransformWS, LaggedTransformWS, TransformModifySettings.AxesToModify, ProcessSpace);
	GetCameraViewData()->CameraTransform = AxesedTransformWS;
	
	LastUpdateTransform = AxesedTransformWS;
}

void UVSCameraFeature_ModifyTransform::CatchUpMovement()
{
	const FRotator& ProcessSpace = GetCameraRelatedRotationByType(TransformModifySettings.ModifySpaceType, TransformModifySettings.CustomModifySpace.Rotator());
	const FTransform& CameraTransformWS = GetCameraViewData()->CameraTransform;
	const FTransform& TargetTransformWS = GetCameraRelatedTransformByType(ModifyTransformType, CustomTransform);
	const FTransform& AxesedTransformWS = UVSMathLibrary::TransformApplyAxes(CameraTransformWS, TargetTransformWS, TransformModifySettings.AxesToModify, ProcessSpace);
	GetCameraViewData()->CameraTransform = AxesedTransformWS;
	LastUpdateTransform = AxesedTransformWS;
}
