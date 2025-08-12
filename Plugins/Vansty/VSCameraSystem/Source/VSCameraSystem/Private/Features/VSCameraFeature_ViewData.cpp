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
	Super::UpdateCamera_Implementation(DeltaTime);

	SyncViewData();
}

void UVSCameraFeature_SyncViewData::SyncViewData_Implementation()
{
	UCameraComponent* CameraComponent = GetCameraComponent();
	UVSCameraViewData* CameraViewData = GetCameraViewData();

	CameraViewData->SyncViewDataFromCamera(CameraComponent);
}



UVSCameraFeature_ApplyViewData::UVSCameraFeature_ApplyViewData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraFeature_ApplyViewData::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);

	ApplyViewData();
}

void UVSCameraFeature_ApplyViewData::ApplyViewData_Implementation()
{
	UCameraComponent* CameraComponent = GetCameraComponent();
	UVSCameraViewData* CameraViewData = GetCameraViewData();

	CameraViewData->ApplyViewDataToCamera(CameraComponent);
}

