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
	virtual void Initialize_Implementation() override;
	virtual bool CanUpdateCamera_Implementation() const override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Transform")
	void CatchUpMovement();
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Rotation")
	FVSCameraRotatorModifySettings RotationModifySettings;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Rotation")
	FVSCameraRotatorLagSettings RotationLagSettings;

	/** Find a player controller by default if not none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	int32 DefaultPlayerIndex = INDEX_NONE;

	/** Assign your custom controller here. */
	UPROPERTY(BlueprintReadWrite, Category = "Controller")
	TWeakObjectPtr<AController> Controller;
	
private:
	FRotator LastUpdatedRotation = FRotator::ZeroRotator;
};
