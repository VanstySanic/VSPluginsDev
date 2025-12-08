// Copyright VanstySanic. All Rights Reserved.

#include "Classes/AlphaBlends/VSAlphaBlendPlayer.h"
#include "Classes/VSTickableObject.h"
#include "Classes/AlphaBlends/VSAlphaBlendPlayCallbackProxy.h"

UVSAlphaBlendPlayer::UVSAlphaBlendPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSAlphaBlendPlayer::Initialize()
{
	if (bHasBeenInitialized) return;
	bHasBeenInitialized = true;
	
	RegisterTickFunction();
	Reset(false);
}

void UVSAlphaBlendPlayer::Destroy()
{
	if (!IsValid(this)) return;
	
	SetAutoUpdate(false);

	if (IsRooted())
	{
		RemoveFromRoot();
	}
	MarkAsGarbage();
}

void UVSAlphaBlendPlayer::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (bIsAutoUpdating && !HasFinished())
	{
		Update(DeltaTime);
	}
}

bool UVSAlphaBlendPlayer::HasFinished() const
{
	return (CurrentDirection == ETimelineDirection::Forward && CurrentTime >= AlphaBlendArgs.BlendTime) || (CurrentDirection == ETimelineDirection::Backward && CurrentTime <= 0.f);
}

void UVSAlphaBlendPlayer::SetAutoUpdate(bool bEnabled)
{
	if (bIsAutoUpdating == bEnabled) return;
	bIsAutoUpdating = bEnabled;

	/** Init update. */
	if (!HasFinished())
	{
		OnUpdated_Native.Broadcast(this, CurrentAlpha);
		OnUpdated.Broadcast(this, CurrentAlpha);
	}
}

void UVSAlphaBlendPlayer::SetTime(float InTime)
{
	const float PrevAlpha = CurrentAlpha;
	const bool PrevHasFinished = HasFinished();
	CurrentTime = FMath::Clamp(InTime, 0.f, AlphaBlendArgs.BlendTime);
	const float LinerAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0.f, AlphaBlendArgs.BlendTime), FVector2D(0.f, 1.f), CurrentTime);
	CurrentAlpha = FAlphaBlend::AlphaToBlendOption(LinerAlpha,AlphaBlendArgs.BlendOption, AlphaBlendArgs.CustomCurve);

	if (PrevAlpha != CurrentAlpha)
	{
		OnUpdated_Native.Broadcast(this, CurrentAlpha);
		OnUpdated.Broadcast(this, CurrentAlpha);
	}
	
	if (!PrevHasFinished && HasFinished())
	{
		FinishInternal();
	}
}

void UVSAlphaBlendPlayer::SetAlpha(float InAlpha)
{
	const float PrevAlpha = CurrentAlpha;
	const bool PrevHasFinished = HasFinished();

	CurrentAlpha = FMath::Clamp(InAlpha, 0.f, 1.f);
	
	float Low = 0.f, High = 1.f;
	while (Low < High)
	{
		float Mid = 0.5f * (Low + High);
		float Test = FAlphaBlend::AlphaToBlendOption(Mid, AlphaBlendArgs.BlendOption, AlphaBlendArgs.CustomCurve);
		if (FMath::IsNearlyEqual(CurrentAlpha, Test, 0.001f))
		{
			break;
		}
		if (Test < CurrentAlpha)
		{
			Low = Mid;
		}
		else
		{
			High = Mid;
		}
	}
	const float LinerAlpha = 0.5f * (Low + High);
	CurrentTime = FMath::Clamp(LinerAlpha * AlphaBlendArgs.BlendTime, 0.f, AlphaBlendArgs.BlendTime);

	if (PrevAlpha != CurrentAlpha)
	{
		OnUpdated_Native.Broadcast(this, CurrentAlpha);
		OnUpdated.Broadcast(this, CurrentAlpha);
	}
	
	if (!PrevHasFinished && HasFinished())
	{
		FinishInternal();
	}
}

void UVSAlphaBlendPlayer::FinishInternal()
{
	OnFinished_Native.Broadcast(this, CurrentAlpha);
	OnFinished.Broadcast(this, CurrentAlpha);
	
	for (const EVSAlphaBlendFinishAction::Type FinishAction : FinishActions)
	{
		switch (FinishAction)
		{
		case EVSAlphaBlendFinishAction::JumpToAnotherPole:
			SetAlpha(1.f - CurrentAlpha);
			break;
		
		case EVSAlphaBlendFinishAction::ReverseDirection:
			ReverseDirection();
			break;
		
		case EVSAlphaBlendFinishAction::StopAutoUpdating:
			SetAutoUpdate(false);
			break;

		case EVSAlphaBlendFinishAction::Destroy:
			Destroy();
			return;
		
		default: ;
		}
	}
}

void UVSAlphaBlendPlayer::Update(float DeltaTime)
{
	const float Time = FMath::Clamp(CurrentTime + DeltaTime * (CurrentDirection == ETimelineDirection::Forward ? 1.f : -1.f), 0.f, AlphaBlendArgs.BlendTime);
	SetTime(Time);
}

void UVSAlphaBlendPlayer::SetDirection(ETimelineDirection::Type Direction)
{
	CurrentDirection = Direction;
}

void UVSAlphaBlendPlayer::ReverseDirection()
{
	CurrentDirection = CurrentDirection == ETimelineDirection::Forward ? ETimelineDirection::Backward : ETimelineDirection::Forward;
}

void UVSAlphaBlendPlayer::Reset(bool bKeepAutoUpdatingState)
{
	SetAlpha(DefaultAlpha);
	SetDirection(DefaultDirection);
	SetAutoUpdate(bKeepAutoUpdatingState ? bIsAutoUpdating : bDefaultAutoUpdate);
}

void UVSAlphaBlendPlayer::SetAlphaBlendArgs(const FAlphaBlendArgs& NewArgs)
{
	AlphaBlendArgs = NewArgs;
	SetAlpha(CurrentAlpha);
}

void UVSAlphaBlendPlayer::SetBlendTime(float BlendTime)
{
	AlphaBlendArgs.BlendTime = BlendTime;
	SetAlpha(CurrentAlpha);
}


/** ------------------------------------------------------------------------- **/


UVSAlphaBlendPlayProxy::UVSAlphaBlendPlayProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AlphaBlendPlayer = CreateDefaultSubobject<UVSAlphaBlendPlayer>(TEXT("AlphaBlendPlayer"));
}

void UVSAlphaBlendPlayProxy::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	/** Sync the time interval. */
	if (AlphaBlendPlayer && AlphaBlendPlayer->GetTickTimeInterval() != GetTickTimeInterval())
	{
		AlphaBlendPlayer->SetTickTimeInterval(GetTickTimeInterval());
	}

	if (RemainedInitialDelay > 0.f)
	{
		RemainedInitialDelay -= DeltaTime;
		if (RemainedInitialDelay <= 0.f)
		{
			RemainedInitialDelay = -1.f;
			CurrentLoopCount++;
			OnLoopStart_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
			OnLoopStart.Broadcast(this, GetAlpha(), CurrentLoopCount);
			AlphaBlendPlayer->SetAutoUpdate(true);
		}
	}

	if (RemainedDirectionTimeInterval > 0.f)
	{
		RemainedDirectionTimeInterval -= DeltaTime;
		if (RemainedDirectionTimeInterval <= 0.f)
		{
			RemainedDirectionTimeInterval = -1.f;
			if (AlphaBlendPlayer)
			{
				AlphaBlendPlayer->ReverseDirection();
				AlphaBlendPlayer->SetAutoUpdate(true);
			}
		}
	}
	
	if (RemainedLoopTimeInterval > 0.f)
	{
		RemainedLoopTimeInterval -= DeltaTime;
		if (RemainedLoopTimeInterval <= 0.f)
		{
			RemainedLoopTimeInterval = -1.f;
			CheckNextLoop();
		}
	}
}

bool UVSAlphaBlendPlayProxy::CanExecuteTick_Implementation() const
{
	return !bIsPaused && !bHasFinished;
}

UVSAlphaBlendPlayProxy* UVSAlphaBlendPlayProxy::CreateAlphaBlendPlayProxy(UObject* WorldContext, const FVSAlphaBlendProxyParams& Params)
{
	if (!WorldContext || !WorldContext->GetWorld()) return nullptr;

	UVSAlphaBlendPlayProxy* Proxy = NewObject<UVSAlphaBlendPlayProxy>(WorldContext);
	Proxy->Initialize(Params);
	
	return Proxy;
}

float UVSAlphaBlendPlayProxy::GetAlpha() const
{
	return AlphaBlendPlayer ? AlphaBlendPlayer->GetAlpha() : 0.f;
}

void UVSAlphaBlendPlayProxy::Reset(bool bIgnoreInitialDelay, bool bKeepUpdateTimeInterval, bool bKeepPauseState)
{
	bHasFinished = false;
	CurrentLoopCount = 0;
	RemainedDirectionTimeInterval = -1.f;
	RemainedLoopTimeInterval = -1.f;
	RemainedInitialDelay = bIgnoreInitialDelay ? -1.f : CachedProxyParams.InitialDelay;
	if (!bKeepUpdateTimeInterval)
	{
		SetTickTimeInterval(CachedProxyParams.UpdateTimeInterval);
	}
	SetPaused(bKeepPauseState ? bIsPaused : CachedProxyParams.bStartPaused);
	
	if (AlphaBlendPlayer)
	{
		AlphaBlendPlayer->Reset(bKeepPauseState);
		AlphaBlendPlayer->SetTickTimeInterval(GetTickTimeInterval());
		AlphaBlendPlayer->SetAutoUpdate(!bIsPaused && RemainedInitialDelay <= 0.f);
	}

	if (bIgnoreInitialDelay || CachedProxyParams.InitialDelay <= 0.f)
	{
		CurrentLoopCount++;
		OnLoopStart_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
		OnLoopStart.Broadcast(this, GetAlpha(), CurrentLoopCount);
	}
}

void UVSAlphaBlendPlayProxy::SetPaused(bool bPaused)
{
	if (bIsPaused == bPaused) return;
	
	bIsPaused = bPaused;
	if (AlphaBlendPlayer)
	{
		AlphaBlendPlayer->SetAutoUpdate(!bIsPaused && RemainedLoopTimeInterval <= 0.f && RemainedDirectionTimeInterval <= 0.f);
	}
	SetTickFunctionEnabled(!bIsPaused);
}

void UVSAlphaBlendPlayProxy::Initialize(const FVSAlphaBlendProxyParams& Params)
{
	CachedProxyParams = Params;

	SetTickTimeInterval(CachedProxyParams.UpdateTimeInterval);
	RegisterTickFunction();
	
	if (AlphaBlendPlayer)
	{
		AlphaBlendPlayer->SetAlphaBlendArgs(CachedProxyParams.AlphaBlendArgs);
		AlphaBlendPlayer->SetTickTimeInterval(CachedProxyParams.UpdateTimeInterval);
		AlphaBlendPlayer->DefaultAlpha = CachedProxyParams.StartAlpha;
		AlphaBlendPlayer->DefaultDirection = CachedProxyParams.StartDirection;
		AlphaBlendPlayer->bDefaultAutoUpdate = false;

		/** Bind delegates here. */
		AlphaBlendPlayer->OnUpdated_Native.AddLambda([this] (UVSAlphaBlendPlayer* Feature, float Alpha)
		{
			OnAlphaUpdated_Native.Broadcast(this, Alpha, CurrentLoopCount);
			OnAlphaUpdated.Broadcast(this, Alpha, CurrentLoopCount);
		});
		AlphaBlendPlayer->OnFinished_Native.AddLambda([this] (UVSAlphaBlendPlayer* Feature, float Alpha)
		{
			AlphaBlendPlayerFinished();
		});
		
		AlphaBlendPlayer->Initialize();
		AlphaBlendPlayer->SetAutoUpdate(!CachedProxyParams.bStartPaused && CachedProxyParams.InitialDelay <= 0.f);
	}

	Reset(false, true, false);
}

void UVSAlphaBlendPlayProxy::Destroy()
{
	if (!IsValid(this)) return;
	
	SetPaused(true);
	UnregisterTickFunction();
	
	if (AlphaBlendPlayer)
	{
		AlphaBlendPlayer->Destroy();
		AlphaBlendPlayer = nullptr;
	}

	if (UVSAlphaBlendPlayCallBackProxy* CallBackProxy = GetTypedOuter<UVSAlphaBlendPlayCallBackProxy>())
	{
		CallBackProxy->SetReadyToDestroy();
	}

	if (IsRooted())
	{
		RemoveFromRoot();
	}
	MarkAsGarbage();
}

void UVSAlphaBlendPlayProxy::AlphaBlendPlayerFinished()
{
	AlphaBlendPlayer->SetAutoUpdate(false);

	OnLoopFinished_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
	OnLoopFinished.Broadcast(this, GetAlpha(), CurrentLoopCount);
	
	bool bDesireNextLoop = true;
	if (CachedProxyParams.bLoopCycleRequiresReversingDirection)
	{
		/** Delay and reverse the direction. */
		if ((AlphaBlendPlayer->GetAlpha() >= 1.f && CachedProxyParams.StartDirection == ETimelineDirection::Forward)
			|| AlphaBlendPlayer->GetAlpha() <= 0.f && CachedProxyParams.StartDirection == ETimelineDirection::Backward)
		{
			bDesireNextLoop = false;

			if (CachedProxyParams.DirectionReverseTimeInterval > 0.f)
			{
				RemainedDirectionTimeInterval = CachedProxyParams.DirectionReverseTimeInterval;
			}
			else
			{
				AlphaBlendPlayer->ReverseDirection();
				AlphaBlendPlayer->SetAutoUpdate(true);
			}
		}
	}
	
	if (bDesireNextLoop)
	{
		if (CachedProxyParams.LoopCount <= 0 || CurrentLoopCount < CachedProxyParams.LoopCount)
		{
			/** Delay and go to the next loop. */
			if (CachedProxyParams.LoopTimeInterval > 0.f)
			{
				RemainedLoopTimeInterval = CachedProxyParams.LoopTimeInterval;
			}
			/** Directly check the next loop. */
			else
			{
				CheckNextLoop();
			}
		}
		else
		{
			DoFinishLogics();
		}
	}
}

void UVSAlphaBlendPlayProxy::CheckNextLoop()
{
	if (CachedProxyParams.LoopCount <= 0 || CurrentLoopCount < CachedProxyParams.LoopCount)
	{
		CurrentLoopCount++;
		if (CachedProxyParams.bLoopCycleRequiresReversingDirection)
		{
			AlphaBlendPlayer->ReverseDirection();
		}
		else
		{
			AlphaBlendPlayer->SetAlpha(1.f - AlphaBlendPlayer->GetAlpha());
		}
		AlphaBlendPlayer->SetAutoUpdate(true);
		
		OnLoopStart_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
		OnLoopStart.Broadcast(this, GetAlpha(), CurrentLoopCount);
	}
	/** Proxy finish. */
	else
	{
		DoFinishLogics();
	}
}

void UVSAlphaBlendPlayProxy::DoFinishLogics()
{
	bHasFinished = true;

	if (AlphaBlendPlayer)
	{
		AlphaBlendPlayer->SetAutoUpdate(false);
	}
	
	if (CachedProxyParams.bPauseWhenFinished)
	{
		SetPaused(true);
	}
	
	OnProxyFinished_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
	OnProxyFinished.Broadcast(this, GetAlpha(), CurrentLoopCount);

	if (CachedProxyParams.bDestroyWhenFinished)
	{
		Destroy();
	}
}
