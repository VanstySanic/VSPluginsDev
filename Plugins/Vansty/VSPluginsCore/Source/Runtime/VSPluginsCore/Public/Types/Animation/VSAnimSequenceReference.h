// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSAnimSequenceReference.generated.h"

USTRUCT(BlueprintType)
struct VSPLUGINSCORE_API FVSAnimSequenceReference : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	bool IsValid() const;
	bool Validate();
	FVector2D GetSafePlayTimeRange() const;
	bool HasTimeMark(const FName MarkName) const;
	float GetMarkTime(const FName MarkName) const;
	bool HasTimePeriod(const FName PeriodName) const;
	FVector2D GetPeriodTimeRange(const FName PeriodName) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> AnimSequence;

protected:
	/**
	 * The desired play time range of this animation.
	 * Do not get it directly.
	 * Use GetSafePlayTimeRange function instead.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D PlayTimeRange = FVector2D(0.f, 65536.f);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> TimeMarks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FVector2D> TimePeriods;
};
