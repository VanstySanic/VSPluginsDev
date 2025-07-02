// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraTypes.h"
#include "VSCameraFeature_AddOffsetTransform.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.Basic.AddOffsetTransform")
class VSCAMERASYSTEM_API UVSCameraFeature_AddOffsetTransform : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	/**
	 * The offset translation, rotation and scale to add.
	 * Notice that the three offsets are seperated.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Offset")
	FTransform OffsetTransform;
	
	/** If true, the scale offset will be multiplied with the original scale instead of being added. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Offset")
	bool bScaleMultiplyInsteadOfAdd = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Transform")
	FVSCameraTransformModifySettings TransformModifySettings;
};
