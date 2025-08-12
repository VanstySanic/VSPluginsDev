// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSGameplayTypes.h"
#include "UObject/Object.h"
#include "VSMovementAnimTypes.generated.h"

class UCharacterMovementComponent;

USTRUCT(BlueprintType)
struct FVSKeyedAnimSequenceMap
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	/** Only exist in editor do distinguish between maps. */
	UPROPERTY(EditAnywhere)
	FName Name;
#endif
	
	UPROPERTY(EditAnywhere)
	TMap<FVSGameplayTagKey, TObjectPtr<UAnimSequence>> Map;
};

USTRUCT(BlueprintType)
struct FVSKeyedAnimSequenceMapContainer
{
	GENERATED_BODY()

	UAnimSequence* GetAnimSequence(const FVSGameplayTagKey& InKeySet) const;
	
	UPROPERTY(EditAnywhere)
	TArray<FVSKeyedAnimSequenceMap> Maps;
};


USTRUCT(BlueprintType)
struct FVSWalkingMovementAnimSettings : public FTableRowBase
{
	GENERATED_BODY()

	/** Lean amount multiplier will reach 1.f if the real acceleration is at this value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float LeanMaxAcceleration = 1500.f;

	/** Lean amount multiplier will reach 1.f if the speed 2d is at this value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float LeanMaxSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float DirectionChangeInertialBlendingTime = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float StanceGaitChangeInertialBlendingTime = 0.36f;
	
	/**
	 * When starting, the stride will be interpolated from 0.f to 1.f. [StartTime, EndTime].
	 * Used both in common and pivot start phases.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVector2D StartStrideBlendInTimeRange = FVector2D(0.15, 0.35);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVector2D StartPlayRateRange = FVector2D(0.6, 5);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVector2D CyclePlayRateRange = FVector2D(0.8, 1.2);
	
	
	/** <Stance, AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<FGameplayTag, TObjectPtr<UAnimSequence>> IdleAnims;
	
	/** Normally used in idle state. <[FromStance, ToStance], AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<FVSGameplayTagKey, TObjectPtr<UAnimSequence>> StanceTransitionAnims;
	
	/** <Stance, AnimSequence> */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	// TMap<FGameplayTag, TObjectPtr<UAnimSequence>> IdleBreakAnims;
	
	/** <[Stance, Gait, InputDirection (from character's evaluated to the input)], AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FVSKeyedAnimSequenceMapContainer StartAnims;
	
	/** <[Stance, Gait, VelocityDirection (from character's evaluated to the movement)], AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FVSKeyedAnimSequenceMapContainer CycleAnims;
	
	/** <[Stance, Gait, VelocityDirection, InputDirection (from character's evaluated to the input)], AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FVSKeyedAnimSequenceMapContainer PivotAnims;
	
	/** <[Stance, Gait, VelocityDirection (from character's evaluated to the movement)], AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FVSKeyedAnimSequenceMapContainer StopAnims;

	/** Normally additive. Four directions enough. <VelocityDirection, AnimSequence> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<FGameplayTag, TObjectPtr<UAnimSequence>> LeanAnims;
};
