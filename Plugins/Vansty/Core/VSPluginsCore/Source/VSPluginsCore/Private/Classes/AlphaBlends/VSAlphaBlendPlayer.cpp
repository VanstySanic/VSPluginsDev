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

	SetDirection(DefaultDirection);
	SetAutoUpdate(bDefaultAutoUpdate);
	SetAlpha(DefaultAlpha);
}

void UVSAlphaBlendPlayer::Destroy()
{
	if (!IsValid(this)) return;

	UnregisterTickFunction();
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
}

void UVSAlphaBlendPlayer::SetTime(float InTime)
{
	const float PrevAlpha = CurrentAlpha;
	const bool PrevHasFinished = HasFinished();
	CurrentTime = FMath::Clamp(InTime, 0.f, AlphaBlendArgs.BlendTime);
	const float LinerAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0.f, AlphaBlendArgs.BlendTime), FVector2D(0.f, 1.f), CurrentTime);
	CurrentAlpha = FAlphaBlend::AlphaToBlendOption(LinerAlpha, AlphaBlendArgs.BlendOption, AlphaBlendArgs.CustomCurve);

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
	CurrentTime = GetTimeByAlpha(CurrentAlpha);

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


float UVSAlphaBlendPlayer::GetTimeByAlpha(float InAlpha)
{
	InAlpha = FMath::Clamp(InAlpha, 0.f, 1.f);

	float Low = 0.f, High = 1.f;
	const int32 MaxIterations = 32;
	for (int32 Iter = 0; Iter < MaxIterations && Low < High; ++Iter)
	{
		float Mid = 0.5f * (Low + High);
		float Test = FAlphaBlend::AlphaToBlendOption(Mid, AlphaBlendArgs.BlendOption, AlphaBlendArgs.CustomCurve);
		if (FMath::IsNearlyEqual(InAlpha, Test))
		{
			break;
		}
		if (Test < InAlpha)
		{
			Low = Mid;
		}
		else
		{
			High = Mid;
		}
	}
	const float LinerAlpha = 0.5f * (Low + High);
	return FMath::Clamp(LinerAlpha * AlphaBlendArgs.BlendTime, 0.f, AlphaBlendArgs.BlendTime);
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
	SetDirection(DefaultDirection);
	SetAutoUpdate(bKeepAutoUpdatingState ? bIsAutoUpdating : bDefaultAutoUpdate);
	SetAlpha(DefaultAlpha);
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

UVSAlphaBlendPlayProxy* UVSAlphaBlendPlayProxy::CreateAlphaBlendPlayProxy(UObject* WorldContext, const FVSAlphaBlendProxyParams& Params, bool bDifferInitialization)
{
	if (!WorldContext || !WorldContext->GetWorld()) return nullptr;

	UVSAlphaBlendPlayProxy* Proxy = NewObject<UVSAlphaBlendPlayProxy>(WorldContext);
	Proxy->CachedProxyParams = Params;
	if (!bDifferInitialization)
	{
		Proxy->Initialize();
	}
	
	return Proxy;
}

float UVSAlphaBlendPlayProxy::GetAlpha() const
{
	return AlphaBlendPlayer ? AlphaBlendPlayer->GetAlpha() : 0.f;
}

void UVSAlphaBlendPlayProxy::Reset(bool bIgnoreInitialDelay, bool bKeepPauseState)
{
	bHasFinished = false;
	CurrentLoopCount = 0;
	RemainedDirectionTimeInterval = -1.f;
	RemainedLoopTimeInterval = -1.f;
	RemainedInitialDelay = bIgnoreInitialDelay ? -1.f : CachedProxyParams.InitialDelay;
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
		AlphaBlendPlayer->SetAutoUpdate(!bIsPaused && RemainedInitialDelay <= 0.f && RemainedLoopTimeInterval <= 0.f && RemainedDirectionTimeInterval <= 0.f);
	}
	SetTickFunctionEnabled(!bIsPaused);
}

void UVSAlphaBlendPlayProxy::Initialize()
{
	if (bHasBeenInitialized) return;
	bHasBeenInitialized = true;

	AlphaBlendPlayer = NewObject<UVSAlphaBlendPlayer>(this);
	check(AlphaBlendPlayer);
	
	if (AlphaBlendPlayer)
	{
		RemainedInitialDelay = CachedProxyParams.InitialDelay;
		bIsPaused = CachedProxyParams.bStartPaused;
	
		SetTickTimeInterval(CachedProxyParams.UpdateTimeInterval);
		SetTickableWhenPaused(CachedProxyParams.bUpdateWhenGamePaused);
		SetTickFunctionEnabled(!CachedProxyParams.bStartPaused);
		RegisterTickFunction();
		
		AlphaBlendPlayer->SetAlphaBlendArgs(CachedProxyParams.AlphaBlendArgs);
		AlphaBlendPlayer->SetTickTimeInterval(CachedProxyParams.UpdateTimeInterval);
		AlphaBlendPlayer->SetTickableWhenPaused(CachedProxyParams.bUpdateWhenGamePaused);
		AlphaBlendPlayer->DefaultAlpha = CachedProxyParams.StartAlpha;
		AlphaBlendPlayer->DefaultDirection = CalcDefaultDirection();
		AlphaBlendPlayer->bDefaultAutoUpdate = !CachedProxyParams.bStartPaused && CachedProxyParams.InitialDelay <= 0.f;

		AlphaBlendPlayer->Initialize();

		/** Start loop during initialization. */
		if (CachedProxyParams.InitialDelay <= 0.f)
		{
			CurrentLoopCount++;
			OnLoopStart_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
			OnLoopStart.Broadcast(this, GetAlpha(), CurrentLoopCount);
		}
		
		OnAlphaUpdated_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
		OnAlphaUpdated.Broadcast(this, GetAlpha(), CurrentLoopCount);
		
		if (AlphaBlendPlayer->HasFinished())
		{
			AlphaBlendPlayerFinished();
		}

		if (AlphaBlendPlayer && IsValid(AlphaBlendPlayer))
		{
			/** Bind delegates here. */
			AlphaBlendPlayer->OnUpdated_Native.AddWeakLambda(this, [this] (UVSAlphaBlendPlayer* Feature, float Alpha)
			{
				OnAlphaUpdated_Native.Broadcast(this, Alpha, CurrentLoopCount);
				OnAlphaUpdated.Broadcast(this, Alpha, CurrentLoopCount);
			});
			AlphaBlendPlayer->OnFinished_Native.AddWeakLambda(this, [this] (UVSAlphaBlendPlayer* Feature, float Alpha)
			{
				AlphaBlendPlayerFinished();
			});
		}
	}
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

ETimelineDirection::Type UVSAlphaBlendPlayProxy::CalcDefaultDirection() const
{
	return (CachedProxyParams.bLoopRequiresReversingDirection && CachedProxyParams.bLoopDirectionAlreadyReversedAtStart)
			? (CachedProxyParams.StartDirection == ETimelineDirection::Forward ? ETimelineDirection::Backward : ETimelineDirection::Forward)
			: CachedProxyParams.StartDirection.GetValue();
}

void UVSAlphaBlendPlayProxy::AlphaBlendPlayerFinished()
{
	AlphaBlendPlayer->SetAutoUpdate(false);

	OnLoopFinished_Native.Broadcast(this, GetAlpha(), CurrentLoopCount);
	OnLoopFinished.Broadcast(this, GetAlpha(), CurrentLoopCount);
	
	bool bDesireNextLoop = true;
	if (CachedProxyParams.bLoopRequiresReversingDirection)
	{
		const ETimelineDirection::Type DefaultDirection = CalcDefaultDirection();
		
		/** Delay and reverse the direction. */
		if ((AlphaBlendPlayer->GetAlpha() >= 1.f && DefaultDirection == ETimelineDirection::Forward)
			|| (AlphaBlendPlayer->GetAlpha() <= 0.f && DefaultDirection == ETimelineDirection::Backward))
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
		if (CachedProxyParams.bLoopRequiresReversingDirection)
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
