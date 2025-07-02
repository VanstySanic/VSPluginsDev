// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature_FollowActorComponent.h"
#include "VSCameraViewData.h"
#include "Libraries/VSMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Libraries/VSActorLibrary.h"

UVSCameraFeature_FollowActorComponent::UVSCameraFeature_FollowActorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TransformModifySettings.ModifySpaceType = EVSCameraRelatedTransformType::AttachedComponent;
	TransformLagSettings.LagSpaceType = EVSCameraRelatedTransformType::AttachedComponent;
}

void UVSCameraFeature_FollowActorComponent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (DefaultActorToFollow.IsValid())
	{
		FollowActor(DefaultActorToFollow.Get(), DefaultComponentName, DefaultSocketName);
	}
	else
	{
		FollowActor(GetCameraComponent()->GetAttachParentActor(), DefaultComponentName, DefaultSocketName);
	}
	CatchUpMovement();
}

bool UVSCameraFeature_FollowActorComponent::CanUpdateCamera_Implementation() const
{
	return Super::CanUpdateCamera_Implementation() && ComponentToFollow.IsValid();
}

void UVSCameraFeature_FollowActorComponent::UpdateCamera_Implementation(float DeltaTime)
{
	const FRotator& TransformModifySpace = GetCameraRelatedRotationByType(TransformModifySettings.ModifySpaceType, TransformModifySettings.CustomModifySpace.Rotator());
	const FRotator& TransformLagSpace = GetCameraRelatedRotationByType(TransformLagSettings.LagSpaceType, TransformLagSettings.CustomLagSpace);
	const FTransform& FollowedComponentTransformWS = ComponentToFollow->GetSocketTransform(SocketName);
	const FTransform& LaggedTransformWS = UVSMathLibrary::TransformInterpTo(LastUpdateTransform, FollowedComponentTransformWS, DeltaTime, TransformLagSettings.LagSpeedTransform, false, TransformLagSettings.MaxLagTimeSubStepping, TransformLagSpace);
	const FTransform& AxesedTransformWS = UVSMathLibrary::TransformApplyAxes(GetCameraViewData()->CameraTransform, LaggedTransformWS, TransformModifySettings.AxesToModify, TransformModifySpace);
	GetCameraViewData()->CameraTransform = AxesedTransformWS;
	LastUpdateTransform = AxesedTransformWS;
}

void UVSCameraFeature_FollowActorComponent::FollowActor(AActor* Actor, FName ComponentName, FName InSocketName)
{
	if (!Actor)
	{
		ComponentToFollow.Reset();
		return;
	}
	ComponentToFollow = ComponentName.IsNone() ? Actor->GetRootComponent() : Cast<USceneComponent>(UVSActorLibrary::GetActorComponentByName(Actor, ComponentName));
	SocketName = InSocketName;
}

void UVSCameraFeature_FollowActorComponent::FollowComponent(USceneComponent* Component, FName InSocketName)
{
	ComponentToFollow = Component;
	SocketName = InSocketName;
}

void UVSCameraFeature_FollowActorComponent::CatchUpMovement()
{
	if (!ComponentToFollow.IsValid()) return;
	const FRotator& TransformModifySpace = GetCameraRelatedRotationByType(TransformModifySettings.ModifySpaceType, TransformModifySettings.CustomModifySpace.Rotator());
	const FTransform& FollowedComponentTransformWS = ComponentToFollow->GetSocketTransform(SocketName);
	const FTransform& AxesedTransformWS = UVSMathLibrary::TransformApplyAxes(GetCameraViewData()->CameraTransform, FollowedComponentTransformWS, TransformModifySettings.AxesToModify, TransformModifySpace);
	GetCameraViewData()->CameraTransform = AxesedTransformWS;
	LastUpdateTransform = AxesedTransformWS;
}
