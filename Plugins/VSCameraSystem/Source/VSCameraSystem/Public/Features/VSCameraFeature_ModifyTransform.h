// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraTypes.h"
#include "VSCameraFeature_ModifyTransform.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.Basic.ModifyTransform")
class VSCAMERASYSTEM_API UVSCameraFeature_ModifyTransform : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Transform")
	void CatchUpMovement();

public:
	/** Use the world space transform defined by that type. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Transform")
	TEnumAsByte<EVSCameraRelatedTransformType::Type> ModifyTransformType = EVSCameraRelatedTransformType::Custom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (EditCondition = "ModifySpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FTransform CustomTransform = FTransform::Identity;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Transform")
	FVSCameraTransformModifySettings TransformModifySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVSCameraTransformLagSettings TransformLagSettings;
	
protected:
	FTransform LastUpdateTransform;
};
