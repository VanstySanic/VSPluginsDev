// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_DoCollisionTest.h"
#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"

UVSCameraFeature_DoCollisionTest::UVSCameraFeature_DoCollisionTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StartLocationModifySettings.ModifySpaceType = EVSCameraRelatedTransformType::AttachedComponent;
}

void UVSCameraFeature_DoCollisionTest::UpdateCamera_Implementation(float DeltaTime)
{
	const FVector& StartLocation = GetCameraRelatedTransformByType(StartLocationModifySettings.ModifySpaceType, StartLocationModifySettings.CustomModifySpace).GetTranslation();
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetCameraComponent()->GetOwner());
	CollisionQueryParams.AddIgnoredActor(GetCameraComponent()->GetAttachParentActor());
	FHitResult HitResult;
	GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		GetCameraViewData()->CameraTransform.GetLocation(),
		FQuat(),
		ProbeChannel,
		FCollisionShape::MakeSphere(ProbeRadius),
		CollisionQueryParams);
	
	if (HitResult.IsValidBlockingHit())
	{
		GetCameraViewData()->CameraTransform.SetLocation(HitResult.Location);
	}
}
