// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSCameraFeature_PreciseMouseDrag.generated.h"

struct FVSLocationUnderCursorQueryParams;
/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.PreciseMouseDrag")
class VSCAMERASYSTEM_API UVSCameraFeature_PreciseMouseDrag : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool CanUpdateCamera_Implementation() const override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;
	virtual void OnFeatureDeactivated_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "PreciseMouseDrag", meta = (AutoCreateRefTerm = "MovementPlaneNormal, QueryParams"))
	void SetPreciseMouseDragAnchor(const FVector& MovementPlaneNormal, const FVSLocationUnderCursorQueryParams& QueryParams);
	
	UFUNCTION(BlueprintCallable, Category = "PreciseMouseDrag")
	void RemovePreciseMouseDragAnchor();

	UFUNCTION(BlueprintCallable, Category = "Zoom")
	void ClearRemainedMovement();
	
public:
	/** Lag speed of the movement. Set to 0.f to disable lagging. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreciseMouseDrag")
	float MovementLagSpeed = 10.f;

	/**
	 * The time sub-stepping in lagging process. This will make the lag look smoother, but may do some cost.
	 * Set to 0.f or negative to disable sub-stepping in lag process.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreciseMouseDrag")
	float MaxLagTimeSubStepping = 0.0166667f;

private:
	FVSLocationUnderCursorQueryParams LocationUnderCursorQueryParams;
	FVector AnchorLocationWorld = FVector::ZeroVector;
	FVector MovementPlaneNormalWorld = FVector::ZeroVector;
	FVector RemainedZoomRelativeMovementNoScale = FVector::ZeroVector;
};
