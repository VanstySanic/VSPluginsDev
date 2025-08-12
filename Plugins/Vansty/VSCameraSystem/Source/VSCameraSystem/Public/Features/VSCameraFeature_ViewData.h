// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraFeature_ViewData.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.ViewData.Sync")
class VSCAMERASYSTEM_API UVSCameraFeature_SyncViewData : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void UpdateCamera_Implementation(float DeltaTime) override;
	
public:
	/** Sync the view data from camera component. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void SyncViewData();
};



/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.ViewData.Apply")
class VSCAMERASYSTEM_API UVSCameraFeature_ApplyViewData : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

public:
	/** Apply view data to camera component. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
	void ApplyViewData();

protected:
	virtual void UpdateCamera_Implementation(float DeltaTime) override;
};