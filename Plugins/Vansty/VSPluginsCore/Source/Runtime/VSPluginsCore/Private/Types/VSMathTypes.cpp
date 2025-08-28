// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSMathTypes.h"
#include "Math/UnrealMathUtility.h"

void FVSAlphaDurationBlend::Update(float InDeltaTime)
{
	if (InDeltaTime <= 0.f) return;

	if (HasFinished())
	{
		Alpha = 0.f;
		return;
	}
	
	const float InTime   = FMath::Max(0.f, AlphaDurationBlendArgs.AlphaBlendInArgs.BlendTime);
	const float HoldTime = FMath::Max(0.f, AlphaDurationBlendArgs.Duration);
	const float OutTime  = FMath::Max(0.f, AlphaDurationBlendArgs.AlphaBlendOutArgs.BlendTime);

	UpdatedTime = FMath::Clamp(UpdatedTime + InDeltaTime, 0.f, InTime + HoldTime + OutTime);

	
	/** Blend in */
	if (UpdatedTime < InTime)
	{
		const float LinerAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0.f, InTime), FVector2D(0.f, 1.f), UpdatedTime);
		Alpha = FAlphaBlend::AlphaToBlendOption(LinerAlpha, AlphaDurationBlendArgs.AlphaBlendInArgs.BlendOption, AlphaDurationBlendArgs.AlphaBlendInArgs.CustomCurve);
	}
	/** Keep alpha 1.f. */
	else if (UpdatedTime <= InTime + HoldTime)
	{
		Alpha = 1.f;
	}
	/** Blend out. */
	else
	{
		const float LinerAlphaReversed = FMath::GetMappedRangeValueClamped(FVector2D(InTime + HoldTime, InTime + HoldTime + OutTime), FVector2D(0.f, 1.f), UpdatedTime);
		const float AlphaReversed = FAlphaBlend::AlphaToBlendOption(LinerAlphaReversed, AlphaDurationBlendArgs.AlphaBlendInArgs.BlendOption, AlphaDurationBlendArgs.AlphaBlendInArgs.CustomCurve);
		Alpha = 1.f - AlphaReversed;
	}
}

bool FVSAlphaDurationBlend::HasFinished() const
{
	const float InTime   = FMath::Max(0.f, AlphaDurationBlendArgs.AlphaBlendInArgs.BlendTime);
	const float HoldTime = FMath::Max(0.f, AlphaDurationBlendArgs.Duration);
	const float OutTime  = FMath::Max(0.f, AlphaDurationBlendArgs.AlphaBlendOutArgs.BlendTime);

	return UpdatedTime >= InTime + HoldTime + OutTime;
}

void FVSAlphaDurationBlend::Reset()
{
	Alpha = 0.f;
	UpdatedTime = 0.f;
}
