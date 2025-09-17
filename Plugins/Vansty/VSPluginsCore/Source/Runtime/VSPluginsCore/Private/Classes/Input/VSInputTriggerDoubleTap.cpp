// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Input/VSInputTriggerDoubleTap.h"

UVSInputTriggerDoubleTap::UVSInputTriggerDoubleTap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldAlwaysTick = true;
}

ETriggerState UVSInputTriggerDoubleTap::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	const bool bCurActuated = IsActuated(ModifiedValue);
	const bool bLastActuated = IsActuated(LastValue);
	
	if (bCurActuated)
	{
		HeldDuration = CalculateHeldDuration(PlayerInput, DeltaTime);
	}
	else
	{
		HeldDuration = 0.0f;
	}
	
	if (HeldDuration >= SecondTapTimeThreshold)
	{
		bPendingTrigger = false;
		FirstTapAccumulatedTime = 0.f;
		return ETriggerState::None;
	}
	
	if (bPendingTrigger)
	{
		FirstTapAccumulatedTime += DeltaTime;
		if (FirstTapAccumulatedTime > SecondTapTimeThreshold)
		{
			bPendingTrigger = false;
			FirstTapAccumulatedTime = 0.f;
			return ETriggerState::None;
		}
	}
	
	if (!bLastActuated && bCurActuated)
	{
		FirstTapAccumulatedTime = 0.f;
		if (!bPendingTrigger)
		{
			bPendingTrigger = true;
			return ETriggerState::Ongoing;
		}
		else
		{
			bPendingTrigger = false;
			return ETriggerState::Triggered;
		}
	}
	if (bPendingTrigger)
	{
		return ETriggerState::Ongoing;
	}
	
	return ETriggerState::None;
}
