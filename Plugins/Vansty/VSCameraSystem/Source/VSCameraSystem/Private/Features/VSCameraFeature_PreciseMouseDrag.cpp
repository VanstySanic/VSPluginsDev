// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_PreciseMouseDrag.h"
#include "VSCameraViewData.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Libraries/VSMathLibrary.h"

UVSCameraFeature_PreciseMouseDrag::UVSCameraFeature_PreciseMouseDrag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSCameraFeature_PreciseMouseDrag::CanUpdateCamera_Implementation() const
{
	return Super::CanUpdateCamera_Implementation()
		&& (LocationUnderCursorQueryParams.PlayerController.IsValid()
			|| !RemainedRelativeMovementNoScale.IsNearlyZero(0.01f));
}

void UVSCameraFeature_PreciseMouseDrag::UpdateCamera_Implementation(float DeltaTime)
{
	Super::UpdateCamera_Implementation(DeltaTime);

	UVSCameraViewData* CameraViewData = GetCameraViewData();
	const FTransform& ViewTransform = CameraViewData->GetViewTransform();

	/** Update remained movement if the anchor is set. */
	if (LocationUnderCursorQueryParams.PlayerController.IsValid())
	{
		FVector LocationUnderCursor;
		if (UVSGameplayLibrary::GetLocationUnderCursor(LocationUnderCursor, LocationUnderCursorQueryParams))
		{
			RemainedRelativeMovementNoScale = ViewTransform.InverseTransformVectorNoScale(FVector::VectorPlaneProject(AnchorLocationWorld - LocationUnderCursor, MovementPlaneNormalWorld));
		}
	}

	const float LaggedScale = UVSMathLibrary::FloatInterpTo(1.f, 0.f, MovementLagSpeed, MaxLagTimeSubStepping);
	const FVector& MovementRelative = RemainedRelativeMovementNoScale * (1.f - LaggedScale);
	const FVector& MovementWorld = ViewTransform.TransformVectorNoScale(MovementRelative);
	
	CameraViewData->CameraTransform.SetTranslation(CameraViewData->CameraTransform.GetTranslation() + MovementWorld);
	RemainedRelativeMovementNoScale *= LaggedScale;
}

void UVSCameraFeature_PreciseMouseDrag::OnFeatureDeactivated_Implementation()
{
	ClearRemainedMovement();

	Super::OnFeatureDeactivated_Implementation();
}

void UVSCameraFeature_PreciseMouseDrag::SetPreciseMouseDragAnchor(const FVector& MovementPlaneNormal, const FVSLocationUnderCursorQueryParams& QueryParams)
{
	if (!IsActive() || !QueryParams.IsValid()) return;

	FVector NewAnchorLocation;
	if (!UVSGameplayLibrary::GetLocationUnderCursor(NewAnchorLocation, QueryParams)) return;

	if (LocationUnderCursorQueryParams.PlayerController.IsValid())
	{
		if (QueryParams.PlayerController != LocationUnderCursorQueryParams.PlayerController.Get())
		{
			RemovePreciseMouseDragAnchor();
		}
	}

	/** Reset the remained movement when set new anchor. */
	RemainedRelativeMovementNoScale = FVector::ZeroVector;

	AnchorLocationWorld = NewAnchorLocation;
	MovementPlaneNormalWorld = MovementPlaneNormal;
	
	LocationUnderCursorQueryParams = QueryParams;
	LocationUnderCursorQueryParams.bTraceByCollision = false;
	LocationUnderCursorQueryParams.bIntersectByPlane = true;
	LocationUnderCursorQueryParams.PlaneNormal = MovementPlaneNormal;
	LocationUnderCursorQueryParams.PlanePoint = AnchorLocationWorld;

	UVSGameplayLibrary::GetLocationUnderCursor(NewAnchorLocation, QueryParams);
}

void UVSCameraFeature_PreciseMouseDrag::RemovePreciseMouseDragAnchor()
{
	LocationUnderCursorQueryParams = FVSLocationUnderCursorQueryParams();
	AnchorLocationWorld = FVector::ZeroVector;
	MovementPlaneNormalWorld = FVector::ZeroVector;
}

void UVSCameraFeature_PreciseMouseDrag::ClearRemainedMovement()
{
	RemainedRelativeMovementNoScale = FVector::ZeroVector;
}