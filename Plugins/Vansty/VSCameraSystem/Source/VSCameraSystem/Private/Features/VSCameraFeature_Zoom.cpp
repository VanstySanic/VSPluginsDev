// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_Zoom.h"
#include "VSCameraViewData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Libraries/VSMathLibrary.h"

UVSCameraFeature_Zoom::UVSCameraFeature_Zoom(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool UVSCameraFeature_Zoom::CanUpdateCamera_Implementation() const
{
	return Super::CanUpdateCamera_Implementation() && !RemainedZoomRelativeMovementNoScale.IsNearlyZero(0.01f);
}

void UVSCameraFeature_Zoom::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);

	UVSCameraViewData* CameraViewData = GetCameraViewData();
	const FTransform& ViewTransform = CameraViewData->GetViewTransform();

	const float LaggedZoomScale = UVSMathLibrary::FloatInterpTo(1.f, 0.f, MovementLagSpeed, MaxLagTimeSubStepping);
	const FVector& ZoomMovementRelative = RemainedZoomRelativeMovementNoScale * (1.f - LaggedZoomScale);
	const FVector& ZoomMovementWorld = ViewTransform.TransformVectorNoScale(ZoomMovementRelative);

	CameraViewData->CameraTransform.SetTranslation(CameraViewData->CameraTransform.GetTranslation() + ZoomMovementWorld);
	RemainedZoomRelativeMovementNoScale *= LaggedZoomScale;
}

void UVSCameraFeature_Zoom::OnFeatureDeactivated_Implementation()
{
	ClearRemainedZoom();

	Super::OnFeatureDeactivated_Implementation();
}

void UVSCameraFeature_Zoom::ZoomAtTarget(float ZoomValue, const FVector& TargetLocation)
{
	ZoomInternal(ZoomValue, TargetLocation, false);
}

void UVSCameraFeature_Zoom::ZoomAtCenter(const float ZoomValue, const float TargetDistance)
{
	if (FMath::IsNearlyZero(ZoomValue)) return;
	const FTransform& ViewTransform = GetCameraViewData()->GetViewTransform();
	const FVector& TargetLocation = ViewTransform.GetTranslation() + TargetDistance * ViewTransform.GetRotation().GetForwardVector();
	ZoomInternal(ZoomValue, TargetLocation, true);
}

void UVSCameraFeature_Zoom::ZoomAtMouseCursor(const float ZoomValue, const FVSLocationUnderCursorQueryParams& QueryParams)
{
	if (FMath::IsNearlyZero(ZoomValue)) return;
	FVector TargetLocation;
	if (!UVSGameplayLibrary::GetLocationUnderCursor(TargetLocation, QueryParams)) return;
	ZoomInternal(ZoomValue, TargetLocation, false);
}

void UVSCameraFeature_Zoom::ClearRemainedZoom()
{
	RemainedZoomRelativeMovementNoScale = FVector::ZeroVector;
}

void UVSCameraFeature_Zoom::ZoomInternal(const float ZoomValue, const FVector& TargetLocation, bool bIgnoreHorizontalZoom)
{
	if (FMath::IsNearlyZero(ZoomValue)) return;
	const FTransform& ViewTransform = GetCameraViewData()->GetViewTransform();
	const FVector& ViewForwardDirection = GetCameraViewData()->GetViewTransform().GetRotation().GetForwardVector();
	const FVector& LegacyZoomViewLocation = ViewTransform.GetTranslation() + ViewTransform.TransformVectorNoScale(RemainedZoomRelativeMovementNoScale);

	/** Must zoom at forward target. */
	if ((TargetLocation - LegacyZoomViewLocation).Dot(ViewForwardDirection) < 0.f) return;

	const float ZoomSpeedToUse = FMath::Max(ZoomSpeedFactor, 0.f) + 1.f;
	const float ZoomScale = FMath::Pow(ZoomSpeedToUse, -ZoomValue);
	
	const FVector& DesiredDeltaTargetLocation = (TargetLocation - LegacyZoomViewLocation) * (1.f - ZoomScale);
	const FVector& DesiredCameraViewLocation = LegacyZoomViewLocation + DesiredDeltaTargetLocation;

	const float OriginalDistance = (TargetLocation - LegacyZoomViewLocation).Size();
	const float OriginalVerticalDistance = (TargetLocation - LegacyZoomViewLocation).ProjectOnToNormal(ViewForwardDirection).Size();
	const float DesiredVerticalDistance = (TargetLocation - DesiredCameraViewLocation).ProjectOnToNormal(ViewForwardDirection).Size();

	const FVector& DistanceConstrainPlaneNormal = ZoomDistanceConstrainPlaneNormal.IsNearlyZero() ? ViewForwardDirection : ZoomDistanceConstrainPlaneNormal;
	const FVector& DesiredPointProjectedToPlane = UKismetMathLibrary::ProjectPointOnToPlane(DesiredCameraViewLocation, TargetLocation, DistanceConstrainPlaneNormal);
	const float DesiredPointProjectionDistance = (DesiredPointProjectedToPlane - DesiredCameraViewLocation).Size();
	const float AdjustedPointProjectionDistance = FMath::Clamp(DesiredPointProjectionDistance, ZoomDistanceToPlaneConstrainRange.X, ZoomDistanceToPlaneConstrainRange.Y);

	
	const float AdjustedVerticalDistance = DesiredVerticalDistance * (AdjustedPointProjectionDistance / DesiredPointProjectionDistance);
	
	const float AdjustedDistance = AdjustedVerticalDistance * (OriginalDistance / OriginalVerticalDistance);
	const FVector& TargetCameraViewLocation = TargetLocation - AdjustedDistance * (TargetLocation - LegacyZoomViewLocation).GetSafeNormal();
	
	FVector DeltaTargetRelativeLocationNoScale = ViewTransform.InverseTransformVectorNoScale(TargetCameraViewLocation - LegacyZoomViewLocation);

	if (bIgnoreHorizontalZoom)
	{
		DeltaTargetRelativeLocationNoScale.Y = 0.f;
		DeltaTargetRelativeLocationNoScale.Z = 0.f;
	}
	
	RemainedZoomRelativeMovementNoScale += DeltaTargetRelativeLocationNoScale;
}
