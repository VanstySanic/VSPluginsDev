// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSControlRotationFeature.h"
#include "Libraries/VSActorLibrary.h"
#include "Net/UnrealNetwork.h"

UVSControlRotationFeature::UVSControlRotationFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
}

void UVSControlRotationFeature::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UVSControlRotationFeature, ReplicatedControlRotation, COND_SimulatedOnly);
}

void UVSControlRotationFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	Controller = UVSActorLibrary::GetControllerFromActor(GetOwnerActor());
}

void UVSControlRotationFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (Controller.IsValid() && Controller->IsLocalController())
	{
		ControlRotation = Controller->GetControlRotation();
		ReplicatedControlRotation = ControlRotation;
		SyncControlRotation_Server(ControlRotation);
	}
}

void UVSControlRotationFeature::SetController(AController* NewController)
{
	Controller = NewController;
}

void UVSControlRotationFeature::OnRep_ReplicatedControlRotation()
{
	ControlRotation = ReplicatedControlRotation;
}

void UVSControlRotationFeature::SyncControlRotation_Server_Implementation(const FRotator& Rotation)
{
	ReplicatedControlRotation = Rotation;
	ControlRotation = ReplicatedControlRotation;
}