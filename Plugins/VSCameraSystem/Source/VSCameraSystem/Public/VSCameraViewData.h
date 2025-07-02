// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSCameraViewData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class VSCAMERASYSTEM_API UVSCameraViewData : public UObject
{
	GENERATED_UCLASS_BODY()

	/** World space transform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CameraTransform = FTransform::Identity;
	
	/** Local (camera transform) space transform offset that adjust the view transform (additive) and won't affect camera transform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform AdditiveViewOffsetTransform = FTransform::Identity;
	
	/** FOV. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FieldOfView = 90.f;
	
	/** Offset that adjust the view FOV (additive) and won't affect camera FOV. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdditiveFieldOfViewOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AspectRatio = 16.f / 9.f;
};
