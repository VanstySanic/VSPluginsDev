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

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void BeginPlay_Implementation() override;

private:
	UFUNCTION(Server, Unreliable)
	void SyncControlRotation_Server(const FRotator& Rotation);
	
public:
	/** If there is no camera component / actor in the outer chain, will find a camera with this name in the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName CameraComponentName = NAME_None;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bCameraAbsoluteLocation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bCameraAbsoluteRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bCameraAbsoluteScale = true;

private:
	UPROPERTY(Replicated)
	FRotator ReplicatedControlRotation = FRotator::ZeroRotator;

private:
	UPROPERTY(EditAnywhere, Instanced, Category = "Camera")
	TObjectPtr<UVSCameraViewData> CameraViewData;

	TWeakObjectPtr<UCameraComponent> CameraComponentPrivate;
	TWeakObjectPtr<AController> ControllerPrivate;
};
