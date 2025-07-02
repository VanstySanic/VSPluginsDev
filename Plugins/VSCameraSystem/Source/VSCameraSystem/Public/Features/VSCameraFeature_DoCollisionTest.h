// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraTypes.h"
#include "VSCameraFeature_DoCollisionTest.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.Basic.DoCollisionTest")
class VSCAMERASYSTEM_API UVSCameraFeature_DoCollisionTest : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Test")
	float ProbeRadius = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Test")
	TEnumAsByte<ECollisionChannel> ProbeChannel = ECC_Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVSCameraVectorModifySettings StartLocationModifySettings;
};
