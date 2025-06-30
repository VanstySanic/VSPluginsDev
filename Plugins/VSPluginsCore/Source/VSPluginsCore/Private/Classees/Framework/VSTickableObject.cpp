// Copyright VanstySanic. All Rights Reserved.

#include "Classees/Framework/VSTickableObject.h"

UVSTickableObject::UVSTickableObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAllowTicking(true)
{
	PrimaryObjectTick.bCanEverTick = true;
	PrimaryObjectTick.bStartWithTickEnabled = true;
	PrimaryObjectTick.SetTickFunctionEnable(false);
}

void UVSTickableObject::RegisterTickFunction()
{
	PrimaryObjectTick.RegisterAndSetup(this);
	if (!PrimaryObjectTick.IsTickFunctionRegistered()) return;
	PrimaryObjectTick.CanExecuteTick.BindUObject(this, &UVSTickableObject::CanTick);
	PrimaryObjectTick.OnExecuteTick.BindLambda([this] (float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction)
	{
		Tick(DeltaTime);
		OnTick.Broadcast(DeltaTime);
	});
}

void UVSTickableObject::UnregisterTickFunction()
{
	PrimaryObjectTick.UnregisterAndCleanup();
	PrimaryObjectTick.CanExecuteTick.Unbind();
	PrimaryObjectTick.OnExecuteTick.Unbind();
}

bool UVSTickableObject::CanTick_Implementation() const
{
	return bAllowTicking;
}

void UVSTickableObject::Tick_Implementation(float DeltaTime)
{
	
}