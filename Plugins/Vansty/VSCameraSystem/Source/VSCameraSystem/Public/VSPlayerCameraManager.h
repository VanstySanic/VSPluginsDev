// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraTypes.h"
#include "Camera/PlayerCameraManager.h"
#include "Types/VSMathTypes.h"
#include "VSPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class VSCAMERASYSTEM_API AVSPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

public:
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation /** Should be spaced */, FRotator& OutDeltaRot /** Absolute value */) override;

public:
	/** Defines whether axes can be limited by ProcessViewRotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TEnumAsByte<EVSRotatorAxes::Type> ViewRotationAxesLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TEnumAsByte<EVSCameraRelatedTransformType::Type> ViewRotationProcessSpaceType = EVSCameraRelatedTransformType::AttachedActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModifySpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FRotator CustomViewRotationProcessSpace = FRotator::ZeroRotator;

	FVector LastUpdatedGravity;
};
