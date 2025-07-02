// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraFeatureAgent.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.Agent")
class VSCAMERASYSTEM_API UVSCameraFeatureAgent : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSCameraFeature;

protected:
	virtual void Initialize_Implementation() override;

public:
	/** If there is no camera component / actor in the outer chain, will find a camera with this name in the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName CameraComponentName = NAME_None;
	
private:
	UPROPERTY(EditAnywhere, Instanced, Category = "Camera")
	TObjectPtr<UVSCameraViewData> CameraViewData;

	TWeakObjectPtr<UCameraComponent> CameraComponentPrivate;
};
