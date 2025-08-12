// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeatureAgent.h"
#include "VSCameraViewData.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Libraries/VSActorLibrary.h"

UVSCameraFeatureAgent::UVSCameraFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraViewData = CreateDefaultSubobject<UVSCameraViewData>(TEXT("CameraViewData"));
}

void UVSCameraFeatureAgent::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	CameraComponentPrivate = GetTypedOuter<UCameraComponent>();
	if (!CameraComponentPrivate.IsValid())
	{
		if (ACameraActor* CameraActor = GetTypedOuter<ACameraActor>())
		{
			CameraComponentPrivate = CameraActor->GetCameraComponent();
		}

		if (!CameraComponentPrivate.IsValid())
		{
			if (AActor* OwnerActor = GetOwnerActor())
			{
				CameraComponentPrivate = Cast<UCameraComponent>(UVSActorLibrary::GetActorComponentByName(OwnerActor, CameraComponentName));
			}
		}
	}
	check(CameraComponentPrivate.IsValid());
}

void UVSCameraFeatureAgent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	CameraComponentPrivate->SetAbsolute(bCameraAbsoluteLocation, bCameraAbsoluteRotation, bCameraAbsoluteScale);
}