// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_ViewData.h"
#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"

UVSCameraFeature_SyncViewData::UVSCameraFeature_SyncViewData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraFeature_SyncViewData::UpdateCamera_Implementation(float DeltaTime)
{
	SyncViewData();
}

void UVSCameraFeature_SyncViewData::SyncViewData_Implementation()
{
	UCameraComponent* CameraComponent = GetCameraComponent();
	UVSCameraViewData* CameraViewData = GetCameraViewData();

	CameraViewData->CameraTransform = CameraComponent->GetComponentTransform();
	CameraViewData->FieldOfView = CameraComponent->FieldOfView;
	CameraComponent->GetAdditiveOffset(CameraViewData->AdditiveViewOffsetTransform, CameraViewData->AdditiveFieldOfViewOffset);
	CameraViewData->AspectRatio = CameraComponent->AspectRatio;
}


UVSCameraFeature_ApplyViewData::UVSCameraFeature_ApplyViewData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraFeature_ApplyViewData::UpdateCamera_Implementation(float DeltaTime)
{
	ApplyViewData();
}

void UVSCameraFeature_ApplyViewData::ApplyViewData_Implementation()
{
	UCameraComponent* CameraComponent = GetCameraComponent();
	UVSCameraViewData* CameraViewData = GetCameraViewData();
	
	CameraComponent->SetWorldTransform(CameraViewData->CameraTransform);
	CameraComponent->SetFieldOfView(CameraViewData->FieldOfView);
	CameraComponent->SetAspectRatio(CameraViewData->AspectRatio);
	
	CameraComponent->ClearAdditiveOffset();
	CameraComponent->AddAdditiveOffset(CameraViewData->AdditiveViewOffsetTransform, CameraViewData->AdditiveFieldOfViewOffset);
}

