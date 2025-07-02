// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_UseControlRotation.h"
#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
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

	if (!ControllerPrivate.IsValid())
	{
		ControllerPrivate = UGameplayStatics::GetPlayerController(this, DefaultPlayerIndex);
	}
	if (!ControllerPrivate.IsValid())
	{
		if (APawn* Pawn = Cast<APawn>(GetCameraComponent()->GetOwner())) { ControllerPrivate = Pawn->GetController(); }
	}
	CatchUpMovement();
}

bool UVSCameraFeature_UseControlRotation::CanUpdateCamera_Implementation() const
{
	return Super::CanUpdateCamera_Implementation() && ControllerPrivate.IsValid();
}

void UVSCameraFeature_UseControlRotation::UpdateCamera_Implementation(float DeltaTime)
{
	const FRotator& RotationModifySpace = GetCameraRelatedRotationByType(RotationModifySettings.ModifySpaceType, RotationModifySettings.CustomModifySpace);
	const FRotator& RotationLagSpace = GetCameraRelatedRotationByType(RotationLagSettings.LagSpaceType, RotationLagSettings.CustomLagSpace);
	const FRotator& ControlRotationWS = ControllerPrivate->GetControlRotation().GetNormalized();
	const FRotator& LaggedRotationWS = UVSMathLibrary::RotatorInterpTo(LastUpdatedRotation, ControlRotationWS, DeltaTime, RotationLagSettings.LagSpeedRotator, false, RotationLagSettings.MaxLagTimeSubStepping, RotationLagSpace);
	const FRotator& AxesedRotationWS = UVSMathLibrary::RotatorApplyAxes(GetCameraViewData()->CameraTransform.Rotator(), LaggedRotationWS, RotationModifySettings.AxesToModify, RotationModifySpace);
	GetCameraViewData()->CameraTransform.SetRotation(AxesedRotationWS.Quaternion());
	LastUpdatedRotation = AxesedRotationWS;
}

void UVSCameraFeature_UseControlRotation::CatchUpMovement()
{
	if (!ControllerPrivate.IsValid()) return;
	const FRotator& RotationModifySpace = GetCameraRelatedRotationByType(RotationModifySettings.ModifySpaceType, RotationModifySettings.CustomModifySpace);
	const FRotator& ControlRotationWS = ControllerPrivate->GetControlRotation().GetNormalized();
	const FRotator& AxesedRotationWS = UVSMathLibrary::RotatorApplyAxes(GetCameraViewData()->CameraTransform.Rotator(), ControlRotationWS, RotationModifySettings.AxesToModify, RotationModifySpace);
	GetCameraViewData()->CameraTransform.SetRotation(AxesedRotationWS.Quaternion());
	LastUpdatedRotation = AxesedRotationWS;
}
