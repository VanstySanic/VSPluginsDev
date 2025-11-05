// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCameraFeatureAgent.h"
#include "VSCameraViewData.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Features/VSCameraFeature_ViewData.h"
#include "Libraries/VSActorLibrary.h"
#include "Net/UnrealNetwork.h"

UVSCameraFeatureAgent::UVSCameraFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraViewData = CreateDefaultSubobject<UVSCameraViewData>(TEXT("CameraViewData"));
	AddDefaultSubFeatureByClass(this, UVSCameraFeature_SyncViewData::StaticClass());
	AddDefaultSubFeatureByClass(this, UVSCameraFeature_ApplyViewData::StaticClass());

	SetIsReplicated(true);
}

void UVSCameraFeatureAgent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UVSCameraFeatureAgent, ReplicatedControlRotation, COND_SimulatedOnly);
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

	ControllerPrivate = UVSActorLibrary::GetControllerFromActor(GetOwnerActor());
	// check(!UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && !ControllerPrivate.IsValid());
}

void UVSCameraFeatureAgent::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (ControllerPrivate.IsValid() && ControllerPrivate->IsLocalController())
	{
		ReplicatedControlRotation = ControllerPrivate->GetControlRotation();
		SyncControlRotation_Server(ReplicatedControlRotation);
	}
}

void UVSCameraFeatureAgent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	CameraComponentPrivate->SetAbsolute(bCameraAbsoluteLocation, bCameraAbsoluteRotation, bCameraAbsoluteScale);
}

void UVSCameraFeatureAgent::SyncControlRotation_Server_Implementation(const FRotator& Rotation)
{
	ReplicatedControlRotation = Rotation;
}
