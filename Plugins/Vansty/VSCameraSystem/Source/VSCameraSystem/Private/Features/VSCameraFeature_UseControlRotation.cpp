// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_UseControlRotation.h"
#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Libraries/VSMathLibrary.h"

UVSCameraFeature_UseControlRotation::UVSCameraFeature_UseControlRotation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RotationModifySettings.ModifySpaceType = EVSCameraRelatedTransformType::AttachedComponent;
	RotationLagSettings.LagSpaceType = EVSCameraRelatedTransformType::AttachedComponent;
}

void UVSCameraFeature_UseControlRotation::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	CatchUpMovement();
}

void UVSCameraFeature_UseControlRotation::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);
	
	const FRotator& RotationModifySpace = GetCameraRelatedRotationByType(RotationModifySettings.ModifySpaceType, RotationModifySettings.CustomModifySpace);
	const FRotator& RotationLagSpace = GetCameraRelatedRotationByType(RotationLagSettings.LagSpaceType, RotationLagSettings.CustomLagSpace);
	const FRotator& ControlRotationWS = GetControlRotation();
	const FRotator& LaggedRotationWS = UVSMathLibrary::RotatorInterpTo(LastUpdatedRotation, ControlRotationWS, DeltaTime, RotationLagSettings.LagSpeedRotator, false, RotationLagSettings.MaxLagTimeSubStepping, RotationLagSpace);
	const FRotator& AxesedRotationWS = UVSMathLibrary::RotatorApplyAxes(GetCameraViewData()->CameraTransform.Rotator(), LaggedRotationWS, RotationModifySettings.AxesToModify, RotationModifySpace);
	GetCameraViewData()->CameraTransform.SetRotation(AxesedRotationWS.Quaternion());
	LastUpdatedRotation = AxesedRotationWS;
}

void UVSCameraFeature_UseControlRotation::CatchUpMovement()
{
	const FRotator& RotationModifySpace = GetCameraRelatedRotationByType(RotationModifySettings.ModifySpaceType, RotationModifySettings.CustomModifySpace);
	const FRotator& ControlRotationWS = GetControlRotation();;
	const FRotator& AxesedRotationWS = UVSMathLibrary::RotatorApplyAxes(GetCameraViewData()->CameraTransform.Rotator(), ControlRotationWS, RotationModifySettings.AxesToModify, RotationModifySpace);
	GetCameraViewData()->CameraTransform.SetRotation(AxesedRotationWS.Quaternion());
	LastUpdatedRotation = AxesedRotationWS;
}
