// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Framework/VSTickableObject.h"

UVSTickableObject::UVSTickableObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bCanEverTicking(true)
{
	PrimaryObjectTick.bCanEverTick = true;
	PrimaryObjectTick.bStartWithTickEnabled = true;
	PrimaryObjectTick.SetTickFunctionEnable(false);
}

void UVSTickableObject::RegisterTickFunction()
{
	if (!bCanEverTicking) return;
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
	if (!bCanEverTicking) return;
	PrimaryObjectTick.UnregisterAndCleanup();
	PrimaryObjectTick.CanExecuteTick.Unbind();
	PrimaryObjectTick.OnExecuteTick.Unbind();
}

bool UVSTickableObject::CanTick_Implementation() const
{
	return true;
}

void UVSTickableObject::Tick_Implementation(float DeltaTime)
{
	
}