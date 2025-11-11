// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeatureAgent.h"
#include "VSCameraViewData.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Classes/Features/VSControlRotationFeature.h"
#include "Features/VSCameraFeature_ViewData.h"
#include "Interfaces/VSControlRotationFeatureInterface.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSObjectLibrary.h"
#include "Net/UnrealNetwork.h"

UVSCameraFeatureAgent::UVSCameraFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraViewData = CreateDefaultSubobject<UVSCameraViewData>(TEXT("CameraViewData"));
	AddDefaultSubFeatureByClass(this, UVSCameraFeature_SyncViewData::StaticClass());
	AddDefaultSubFeatureByClass(this, UVSCameraFeature_ApplyViewData::StaticClass());
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
	CameraComponentPrivate->SetAbsolute(bCameraAbsoluteLocation, bCameraAbsoluteRotation, bCameraAbsoluteScale);

	if (GetOwnerActor()->GetClass()->ImplementsInterface(UVSControlRotationFeatureInterface::StaticClass()))
	{
		ControlRotationFeaturePrivate = IVSControlRotationFeatureInterface::Execute_GetControlRotationFeature(GetOwnerActor());
	}
	else
	{
		ControlRotationFeaturePrivate = UVSObjectLibrary::FindFeatureByClassFromObject<UVSControlRotationFeature>(GetOwnerActor());
	}
}