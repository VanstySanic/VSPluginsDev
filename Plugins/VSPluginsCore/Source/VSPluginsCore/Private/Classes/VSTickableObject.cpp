// Copyright VanstySanic. All Rights Reserved.

#include "Classes/VSTickableObject.h"

UVSTickableObject::UVSTickableObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryObjectTick.bCanEverTick = true;
	PrimaryObjectTick.bStartWithTickEnabled = true;
	PrimaryObjectTick.bAllowTickBatching = true;
	PrimaryObjectTick.TickGroup = TG_PrePhysics;
	PrimaryObjectTick.SetTickFunctionEnable(false);
}

UWorld* UVSTickableObject::GetWorld() const
{
	/** If we are a CDO, we must return nullptr instead of calling Outer->GetWorld() to fool UObject::ImplementsGetWorld. */
	if (GetOuter() && !HasAnyFlags(RF_ClassDefaultObject))
	{
		return GetOuter()->GetWorld();
	}

	return nullptr;
}

void UVSTickableObject::RegisterTickFunction()
{
	if (!GetWorld()) return;
	if (IsTickFunctionRegistered()) return;
	PrimaryObjectTick.RegisterAndSetup(GetTypedOuter<AActor>());
	if (!PrimaryObjectTick.IsTickFunctionRegistered()) return;
	PrimaryObjectTick.CanExecuteTick.BindUObject(this, &UVSTickableObject::CanExecuteTick);
	PrimaryObjectTick.OnExecuteTick.BindUObject(this, &UVSTickableObject::TickObject);
}

void UVSTickableObject::UnregisterTickFunction()
{
	if (!IsTickFunctionRegistered()) return;
	PrimaryObjectTick.UnregisterAndCleanup();
	PrimaryObjectTick.CanExecuteTick.Unbind();
	PrimaryObjectTick.OnExecuteTick.Unbind();
}

bool UVSTickableObject::IsTickFunctionRegistered() const
{
	return PrimaryObjectTick.IsTickFunctionRegistered();
}

bool UVSTickableObject::CanExecuteTick_Implementation() const
{
	return true;
}

void UVSTickableObject::Tick_Implementation(float DeltaTime)
{
}

void UVSTickableObject::TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction)
{
	UWorld* World = GetWorld();
	if (!World) return;

	Tick(DeltaTime);

	/**
	 * Update any latent actions we have for this object, this will update even if paused if bUpdateWhilePaused is enabled.
	 * If this tick is skipped on a frame because we've got a TickInterval, our latent actions will be ticked anyway by UWorld::Tick().
	 * Given that, our latent actions don't need to be passed a larger DeltaSeconds to make up the frames that they missed (because they wouldn't have missed any).
	 * So pass in the world's DeltaSeconds value rather than our specific DeltaSeconds value.
	 */
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	LatentActionManager.ProcessLatentActions(this, World->GetDeltaSeconds());
}


/** ------------------------------------------------------------------------- **/


UVSObjectTickProxy::UVSObjectTickProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSObjectTickProxy::TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (OnTick_Native.IsBound())
	{
		OnTick_Native.Broadcast(DeltaTime, TickType, TickFunction);
	}
	if (OnTick.IsBound())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
