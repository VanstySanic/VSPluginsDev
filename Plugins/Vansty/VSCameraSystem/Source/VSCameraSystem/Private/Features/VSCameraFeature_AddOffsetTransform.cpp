// Copyright VanstySanic. All Rights Reserved.


#include "Features/VSCameraFeature_AddOffsetTransform.h"
#include "VSCameraViewData.h"
#include "Libraries/VSMathLibrary.h"

UVSCameraFeature_AddOffsetTransform::UVSCameraFeature_AddOffsetTransform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OffsetTransform.SetScale3D(FVector::ZeroVector);
}

void UVSCameraFeature_AddOffsetTransform::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);

	FTransform TransformModifySpace = GetCameraRelatedTransformByType(TransformModifySettings.ModifySpaceType, TransformModifySettings.CustomModifySpace);
	const FTransform CameraTransformWS = GetCameraViewData()->CameraTransform;
	const FTransform& CameraTransformRS = CameraTransformWS.GetRelativeTransform(TransformModifySpace);
	const FRotator& TargetRotationRS = CameraTransformRS.GetRotation().Rotator() + OffsetTransform.GetRotation().Rotator();
	const FVector& TargetTranslationRS = CameraTransformRS.GetTranslation() + OffsetTransform.GetTranslation();
	const FVector& TargetScaleRS = bScaleMultiplyInsteadOfAdd 
		? CameraTransformRS.GetScale3D() * OffsetTransform.GetScale3D()
		: CameraTransformRS.GetScale3D() + OffsetTransform.GetScale3D();
	const FTransform& TargetTransformRS = FTransform(TargetRotationRS, TargetTranslationRS, TargetScaleRS);
	const FTransform& TargetTransformWS = TargetTransformRS * TransformModifySpace;
	const FTransform& AxesedTransform = UVSMathLibrary::TransformApplyAxes(CameraTransformWS, TargetTransformWS, TransformModifySettings.AxesToModify, TransformModifySettings.CustomModifySpace.Rotator());
	GetCameraViewData()->CameraTransform = AxesedTransform;
}
