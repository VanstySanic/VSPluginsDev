// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraTypes.h"
#include "VSCameraFeature_ConstrainTransform.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.ConstrainTransform")
class VSCAMERASYSTEM_API UVSCameraFeature_ConstrainTransform : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Follow")
	void CatchUpMovement();

protected:
	/** Range min. Lager than range min means inverse. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constrain")
	FTransform ConstrainTransformMin;

	/** Range max. smaller than range min means inverse. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constrain")
	FTransform ConstrainTransformMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVSCameraTransformModifySettings TransformModifySettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVSCameraTransformLagSettings TransformLagSettings;

	/** Whether to constrain the control rotation as well. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	bool bConstrainControlRotation = false;

	/** Find a player controller by default if not none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	int32 DefaultPlayerIndex = INDEX_NONE;
	
	UPROPERTY(BlueprintReadWrite, Category = "Controller")
	TObjectPtr<AController> ControllerPrivate;

protected:
	FTransform LastUpdateTransform;
};
