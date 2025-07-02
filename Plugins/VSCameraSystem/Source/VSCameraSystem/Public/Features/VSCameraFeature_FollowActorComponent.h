// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCameraFeature.h"
#include "VSCameraTypes.h"
#include "VSCameraFeature_FollowActorComponent.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Camera.Basic.FollowActorComponent")
class VSCAMERASYSTEM_API UVSCameraFeature_FollowActorComponent : public UVSCameraFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay_Implementation() override;
	virtual bool CanUpdateCamera_Implementation() const override;
	virtual void UpdateCamera_Implementation(float DeltaTime) override;

public:
	/**
	 * Follow an actor with optional component name.
	 * @param Actor The actor to follow.
	 * @param ComponentName Name of the component to follow. If left empty, follow actor's root component.
	 */
	UFUNCTION(BlueprintCallable, Category = "Follow")
	void FollowActor(AActor* Actor, FName ComponentName = NAME_None, FName InSocketName = NAME_None);

	/**
	 * Follow a scene component.
	 * @param Component The scene component to follow.
	 */
	UFUNCTION(BlueprintCallable, Category = "Follow")
	void FollowComponent(USceneComponent* Component, FName InSocketName);

	UFUNCTION(BlueprintCallable, Category = "Follow")
	void CatchUpMovement();

public:
	/* Default actor to follow. If not assigned, camera will follow the attached actor by default. */
	UPROPERTY(EditAnywhere, Category = "Follow")
	TWeakObjectPtr<AActor> DefaultActorToFollow;

	/*
	 * Default component to follow.
	 * If not assigned, camera will follow the root component by default.
	 * If DefaultActorToFollow is not assigned as well, follow the attached parent component.
	 */
	UPROPERTY(EditAnywhere, Category = "Follow")
	FName DefaultComponentName;

	UPROPERTY(EditAnywhere, Category = "Follow")
	FName DefaultSocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Transform")
	FVSCameraTransformModifySettings TransformModifySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVSCameraTransformLagSettings TransformLagSettings;

private:
	TWeakObjectPtr<USceneComponent> ComponentToFollow;
	FName SocketName = NAME_None;
	FTransform LastUpdateTransform = FTransform::Identity;
};
