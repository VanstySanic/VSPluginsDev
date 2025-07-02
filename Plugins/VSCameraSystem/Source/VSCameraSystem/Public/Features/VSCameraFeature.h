// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraTypes.h"
#include "Classees/Framework/VSObjectFeature.h"
#include "VSCameraFeature.generated.h"

class UCameraComponent;
class UVSCameraViewData;
class UVSCameraFeatureAgent;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "CameraFeature")
class VSCAMERASYSTEM_API UVSCameraFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Camera")
	UVSCameraFeatureAgent* GetCameraFeatureAgent() const { return CameraFeatureAgentPrivate.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Camera")
	UVSCameraViewData* GetCameraViewData() const;

	/** Get a world space transform related with the camera by type. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tools", meta = (AutoCreateRefTerm = "CustomTransform"))
	FTransform GetCameraRelatedTransformByType(const EVSCameraRelatedTransformType::Type TransformType, const FTransform& CustomTransform = FTransform());

	/** Get a world space rotation related with the camera by type. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tools", meta = (AutoCreateRefTerm = "CustomRotation"))
	FRotator GetCameraRelatedRotationByType(const EVSCameraRelatedTransformType::Type TransformType, const FRotator& CustomRotation = FRotator::ZeroRotator);

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	UCameraComponent* GetCameraComponent() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Camera")
	void UpdateCamera(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Camera")
	bool CanUpdateCamera() const;

private:
	TWeakObjectPtr<UVSCameraFeatureAgent> CameraFeatureAgentPrivate;
};
