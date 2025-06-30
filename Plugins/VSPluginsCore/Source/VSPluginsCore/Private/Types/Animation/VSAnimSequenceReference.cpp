// Copyright VanstySanic. All Rights Reserved.

#include "Types/Animation/VSAnimSequenceReference.h"
#include "Kismet/KismetMathLibrary.h"

bool FVSAnimSequenceReference::IsValid() const
{
	if (!AnimSequence) return false;

	const float AnimPlayLength = AnimSequence->GetPlayLength();

	for (const auto& TimeMark : TimeMarks)
	{
		if (!UKismetMathLibrary::InRange_FloatFloat(TimeMark.Value, 0.f, AnimPlayLength)) return false;
	}
	for (const auto& TimePeriod : TimePeriods)
	{
		if (TimePeriod.Value.X > TimePeriod.Value.Y) return false;
		if (!UKismetMathLibrary::InRange_FloatFloat(TimePeriod.Value.X, 0.f, AnimPlayLength)) return false;
		if (!UKismetMathLibrary::InRange_FloatFloat(TimePeriod.Value.Y, 0.f, AnimPlayLength)) return false;
	}
	
	return true;
}

bool FVSAnimSequenceReference::Validate()
{
	if (IsValid()) return true;
	
	if (!AnimSequence) return false;
	
	const float AnimPlayLength = AnimSequence->GetPlayLength();
	PlayTimeRange = GetSafePlayTimeRange();
	
	for (auto& TimeMark : TimeMarks)
	{
		TimeMark.Value = FMath::Clamp(TimeMark.Value, 0.f, AnimPlayLength);
	}
	for (auto& TimePeriod : TimePeriods)
	{
		if (TimePeriod.Value.X > TimePeriod.Value.Y) Swap(TimePeriod.Value.X, TimePeriod.Value.Y);
		TimePeriod.Value.X = FMath::Clamp(TimePeriod.Value.X, 0.f, AnimPlayLength);
		TimePeriod.Value.Y = FMath::Clamp(TimePeriod.Value.Y, 0.f, AnimPlayLength);
	}
	
	return IsValid();
}

FVector2D FVSAnimSequenceReference::GetSafePlayTimeRange() const
{
	if (!AnimSequence) return FVector2D::ZeroVector;
	const float AnimPlayLength = AnimSequence->GetPlayLength();

	FVector2D SafeTimeRange = PlayTimeRange;
	if (SafeTimeRange.X > SafeTimeRange.Y) { Swap(SafeTimeRange.X, SafeTimeRange.Y); }
	SafeTimeRange.X = FMath::Clamp(SafeTimeRange.X, 0.f, AnimPlayLength);
	SafeTimeRange.Y = FMath::Clamp(SafeTimeRange.Y, 0.f, AnimPlayLength);
	return SafeTimeRange;
}

bool FVSAnimSequenceReference::HasTimeMark(const FName MarkName) const
{
	return TimeMarks.Contains(MarkName);
}

float FVSAnimSequenceReference::GetMarkTime(const FName MarkName) const
{
	return TimeMarks.FindRef(MarkName);
}

bool FVSAnimSequenceReference::HasTimePeriod(const FName PeriodName) const
{
	return TimePeriods.Contains(PeriodName);
}

FVector2D FVSAnimSequenceReference::GetPeriodTimeRange(const FName PeriodName) const
{
	return TimePeriods.FindRef(PeriodName);
}