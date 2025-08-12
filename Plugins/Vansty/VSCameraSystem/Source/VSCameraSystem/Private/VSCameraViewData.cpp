// Copyright VanstySanic. All Rights Reserved.

#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"

UVSCameraViewData::UVSCameraViewData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraViewData::SyncViewDataFromCamera_Implementation(UCameraComponent* CameraComponent)
{
	if (!CameraComponent) return;
	CameraTransform = CameraComponent->GetComponentTransform();
	FieldOfView = CameraComponent->FieldOfView;
	CameraComponent->GetAdditiveOffset(AdditiveViewOffsetTransform, AdditiveFieldOfViewOffset);
	AspectRatio = CameraComponent->AspectRatio;
}

void UVSCameraViewData::ApplyViewDataToCamera_Implementation(UCameraComponent* CameraComponent)
{
	if (!CameraComponent) return;

	CameraComponent->SetWorldTransform(CameraTransform);
	CameraComponent->SetFieldOfView(FieldOfView);
	CameraComponent->SetAspectRatio(AspectRatio);
	
	CameraComponent->ClearAdditiveOffset();
	CameraComponent->AddAdditiveOffset(AdditiveViewOffsetTransform, AdditiveFieldOfViewOffset);
}

FTransform UVSCameraViewData::GetViewTransform() const
{
	return CameraTransform * AdditiveViewOffsetTransform;
}
