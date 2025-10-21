// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeature.h"
#include "VSCameraViewData.h"
#include "Camera/CameraComponent.h"
#include "Features/VSCameraFeatureAgent.h"
#include "Libraries/VSObjectLibrary.h"

UVSCameraFeature::UVSCameraFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCameraFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	CameraFeatureAgentPrivate = GetTypedOuter<UVSCameraFeatureAgent>();
	if (!CameraFeatureAgentPrivate.IsValid()) { CameraFeatureAgentPrivate = Cast<UVSCameraFeatureAgent>(this); }
	if (!CameraFeatureAgentPrivate.IsValid()) { CameraFeatureAgentPrivate = FindOwnerFeatureByClass<UVSCameraFeatureAgent>(); }
	if (!CameraFeatureAgentPrivate.IsValid())
	{
		if (UCameraComponent* CameraComponent = GetTypedOuter<UCameraComponent>())
		{
			CameraFeatureAgentPrivate = UVSObjectLibrary::FindFeatureByClassFromObject<UVSCameraFeatureAgent>(CameraComponent);
		}
		else
		{
			CameraFeatureAgentPrivate = UVSObjectLibrary::FindFeatureByClassFromObject<UVSCameraFeatureAgent>(GetOwnerActor());
		}
	}
	check(CameraFeatureAgentPrivate.IsValid());
}

void UVSCameraFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (CanUpdateCamera())
	{
		UpdateCamera(DeltaTime);
	}
}

UVSCameraViewData* UVSCameraFeature::GetCameraViewData() const
{
	return CameraFeatureAgentPrivate.IsValid() ? CameraFeatureAgentPrivate->CameraViewData : nullptr;
}

UCameraComponent* UVSCameraFeature::GetCameraComponent() const
{
	return CameraFeatureAgentPrivate.IsValid() ? CameraFeatureAgentPrivate->CameraComponentPrivate.Get() : nullptr;
}

void UVSCameraFeature::UpdateCamera_Implementation(float DeltaTime)
{

}

bool UVSCameraFeature::CanUpdateCamera_Implementation() const
{
	return IsActive() && CameraFeatureAgentPrivate.IsValid() && GetCameraComponent() && GetCameraViewData();
}


FTransform UVSCameraFeature::GetCameraRelatedTransformByType(const EVSCameraRelatedTransformType::Type TransformType, const FTransform& CustomTransform)
{
	UCameraComponent* Camera = GetCameraComponent();
	FTransform AnsTransform = FTransform::Identity;
	switch (TransformType)
	{
	case EVSCameraRelatedTransformType::None:
	case EVSCameraRelatedTransformType::World:
		AnsTransform = FTransform::Identity;
		break;

	case EVSCameraRelatedTransformType::Local:
		AnsTransform = Camera->GetComponentTransform();
		break;
		
	case EVSCameraRelatedTransformType::LocalView:
		{
			FTransform OffsetTransform;
			float OffsetFOV;
			Camera->GetAdditiveOffset(OffsetTransform, OffsetFOV);
			AnsTransform = OffsetTransform * Camera->GetComponentTransform();
		}
		break;
		
	case EVSCameraRelatedTransformType::Data:
		AnsTransform = GetCameraViewData()->CameraTransform;
		break;
		
	case EVSCameraRelatedTransformType::DataView:
		{
			UVSCameraViewData* ViewData = GetCameraViewData();
			AnsTransform = ViewData->CameraTransform * ViewData->AdditiveViewOffsetTransform;
		}
		break;
		
	case EVSCameraRelatedTransformType::AttachedActor:
		if (AActor* Actor = Camera->GetAttachParentActor()) { AnsTransform = Actor->GetActorTransform(); }
		break;
		
	case EVSCameraRelatedTransformType::AttachedComponent:
		if (USceneComponent* Component = Camera->GetAttachParent()) { AnsTransform = Component->GetComponentTransform(); }
		break;

	case EVSCameraRelatedTransformType::AttachedSocket:
		if (USceneComponent* Component = Camera->GetAttachParent()) { AnsTransform = Component->GetSocketTransform(Camera->GetAttachSocketName()); }
		break;

	case EVSCameraRelatedTransformType::Custom:
		AnsTransform = CustomTransform;
		break;
		
	default: ;
	}

	return AnsTransform;
}

FRotator UVSCameraFeature::GetCameraRelatedRotationByType(const EVSCameraRelatedTransformType::Type TransformType, const FRotator& CustomRotation)
{
	UCameraComponent* Camera = GetCameraComponent();
	if (!Camera) return FRotator::ZeroRotator;
	FRotator AnsRotation = FRotator::ZeroRotator;
	switch (TransformType)
	{
	case EVSCameraRelatedTransformType::None:
	case EVSCameraRelatedTransformType::World:
		AnsRotation = FRotator::ZeroRotator;
		break;

	case EVSCameraRelatedTransformType::Local:
		AnsRotation = Camera->GetComponentRotation();
		break;
		
	case EVSCameraRelatedTransformType::LocalView:
		{
			FTransform OffsetTransform;
			float OffsetFOV;
			Camera->GetAdditiveOffset(OffsetTransform, OffsetFOV);
			AnsRotation = (OffsetTransform * Camera->GetComponentTransform()).Rotator();
		}
		break;
		
	case EVSCameraRelatedTransformType::Data:
		AnsRotation = GetCameraViewData()->CameraTransform.Rotator();
		break;
		
	case EVSCameraRelatedTransformType::DataView:
		{
			UVSCameraViewData* ViewData = GetCameraViewData();
			AnsRotation = (ViewData->CameraTransform * ViewData->AdditiveViewOffsetTransform).Rotator();
		}
		break;
		
	case EVSCameraRelatedTransformType::AttachedActor:
		if (AActor* Actor = Camera->GetAttachParentActor()) { AnsRotation = Actor->GetActorRotation(); }
		break;
		
	case EVSCameraRelatedTransformType::AttachedComponent:
		if (USceneComponent* Component = Camera->GetAttachParent()) { AnsRotation = Component->GetComponentRotation(); }
		break;

	case EVSCameraRelatedTransformType::Custom:
		AnsRotation = CustomRotation;
		break;
		
	default: ;
	}


	return AnsRotation;
}