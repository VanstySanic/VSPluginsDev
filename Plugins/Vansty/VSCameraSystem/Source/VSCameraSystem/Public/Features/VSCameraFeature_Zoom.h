// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraFeature_Zoom.generated.h"

struct FVSLocationUnderCursorQueryParams;

/**
 * Zoom the camera by adjusting the camera location in the view data.
 */
UCLASS(DisplayName = "Feature.Camera.Basic.Zoom")
class VSCAMERASYSTEM_API UVSCameraFeature_Zoom : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool CanUpdateCamera_Implementation() const override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;
	virtual void OnFeatureDeactivated_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Zoom", meta = (AutoCreateRefTerm = "TargetLocation"))
	void ZoomAtTarget(const float ZoomValue, const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Zoom")
	void ZoomAtCenter(const float ZoomValue, const float TargetDistance);

	UFUNCTION(BlueprintCallable, Category = "Zoom", meta = (AutoCreateRefTerm = "QueryParams"))
	void ZoomAtMouseCursor(const float ZoomValue, const FVSLocationUnderCursorQueryParams& QueryParams);
	
	UFUNCTION(BlueprintCallable, Category = "Zoom")
	void ClearRemainedZoom();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (ClampMin = "0"))
	float ZoomSpeedFactor = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	FVector2D ZoomDistanceToPlaneConstrainRange = FVector2D(256.f, 4096.f);
	
	/** If left zero, will use the camera view forward vector. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	FVector ZoomDistanceConstrainPlaneNormal = FVector::ZeroVector;
	
	/** Lag speed of the movement. Set to 0.f to disable lagging. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MovementLagSpeed = 10.f;

	/**
	 * The time sub-stepping in lagging process. This will make the lag look smoother, but may do some cost.
	 * Set to 0.f or negative to disable sub-stepping in lag process.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MaxLagTimeSubStepping = 0.0166667f;

private:
	void ZoomInternal(const float ZoomValue, const FVector& TargetLocation, bool bIgnoreHorizontalZoom = false);

private:
	FVector RemainedZoomRelativeMovementNoScale = FVector::ZeroVector;
};
