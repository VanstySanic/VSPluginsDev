// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSAlphaBlendFeature.h"

UVSAlphaBlendFeature::UVSAlphaBlendFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSAlphaBlendFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	SetAlpha(DefaultAlpha);
	SetUpdateTimeInterval(UpdateTimeInterval);
}

void UVSAlphaBlendFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	if (bDefaultAutoUpdate)
	{
		SetAutoUpdate(true);
	}
}

void UVSAlphaBlendFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (bIsAutoUpdating)
	{
		Update(DeltaTime);
	}
}

bool UVSAlphaBlendFeature::HasFinished() const
{
	return (Direction == ETimelineDirection::Forward && UpdatedTime >= AlphaBlendArgs.BlendTime) || (Direction == ETimelineDirection::Backward && UpdatedTime <= 0.f);
}

void UVSAlphaBlendFeature::SetAutoUpdate(bool bEnabled)
{
	if (bIsAutoUpdating == bEnabled) return;
	bIsAutoUpdating = bEnabled;
}

void UVSAlphaBlendFeature::SetTime(float InTime)
{
	const bool PrevHasFinished = HasFinished();
	UpdatedTime = FMath::Clamp(InTime, 0.f, AlphaBlendArgs.BlendTime);
	const float LinerAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0.f, AlphaBlendArgs.BlendTime), FVector2D(0.f, 1.f), UpdatedTime);
	Alpha = FAlphaBlend::AlphaToBlendOption(LinerAlpha,AlphaBlendArgs.BlendOption, AlphaBlendArgs.CustomCurve);

	OnUpdated.Broadcast(this, Alpha, UpdatedTime);
	
	if (!PrevHasFinished && HasFinished())
	{
		DoFinishedLogic();
	}
}

void UVSAlphaBlendFeature::SetAlpha(float InAlpha)
{
	const bool PrevHasFinished = HasFinished();

	Alpha = FMath::Clamp(InAlpha, 0.f, 1.f);
	
	float Low = 0.f, High = 1.f;
	while (Low < High)
	{
		float Mid = 0.5f * (Low + High);
		float Test = FAlphaBlend::AlphaToBlendOption(Mid, AlphaBlendArgs.BlendOption, AlphaBlendArgs.CustomCurve);
		if (FMath::IsNearlyEqual(Alpha, Test, 0.001f))
		{
			break;
		}
		if (Test < Alpha)
		{
			Low = Mid;
		}
		else
		{
			High = Mid;
		}
	}
	const float LinerAlpha = 0.5f * (Low + High);
	UpdatedTime = FMath::Clamp(LinerAlpha * AlphaBlendArgs.BlendTime, 0.f, AlphaBlendArgs.BlendTime);

	OnUpdated.Broadcast(this, Alpha, UpdatedTime);
	
	if (!PrevHasFinished && HasFinished())
	{
		DoFinishedLogic();
	}
}

void UVSAlphaBlendFeature::SetUpdateTimeInterval(float Interval)
{
	UpdateTimeInterval = Interval;
	PrimaryObjectTick.TickInterval = UpdateTimeInterval;
}

void UVSAlphaBlendFeature::DoFinishedLogic()
{
	OnFinished.Broadcast(this, Alpha, UpdatedTime);
		
	if (bStopAutoUpdateWhenFinished)
	{
		SetAutoUpdate(false);
	}
}

void UVSAlphaBlendFeature::Update(float DeltaTime)
{
	const float Time = FMath::Clamp(UpdatedTime + DeltaTime * (Direction == ETimelineDirection::Forward ? 1.f : -1.f), 0.f, AlphaBlendArgs.BlendTime);
	SetTime(Time);
}

void UVSAlphaBlendFeature::SetDirection(ETimelineDirection::Type InDirection)
{
	Direction = InDirection;
}

void UVSAlphaBlendFeature::ReverseDirection()
{
	Direction = Direction == ETimelineDirection::Forward ? ETimelineDirection::Backward : ETimelineDirection::Forward;
}

void UVSAlphaBlendFeature::SetAlphaBlendArgs(const FAlphaBlendArgs& NewArgs)
{
	AlphaBlendArgs = NewArgs;
	SetAlpha(Alpha);
}
