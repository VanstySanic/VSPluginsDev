// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraTypes.h"
#include "VSCameraFeature_UseControlRotation.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.UseControlRotation")
class VSCAMERASYSTEM_API UVSCameraFeature_UseControlRotation : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Transform")
	void CatchUpMovement();
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Rotation")
	FVSCameraRotatorModifySettings RotationModifySettings;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Rotation")
	FVSCameraRotatorLagSettings RotationLagSettings;

private:
	FRotator LastUpdatedRotation = FRotator::ZeroRotator;
};
