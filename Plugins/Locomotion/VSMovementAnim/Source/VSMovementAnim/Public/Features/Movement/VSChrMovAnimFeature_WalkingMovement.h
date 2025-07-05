// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "Types/VSMovementAnimTypes.h"
#include "VSChrMovAnimFeature_WalkingMovement.generated.h"

class UVSAnimDataAsset_WalkingMovement;
class UVSChrMovFeature_WalkingMovement;
class UVSChrMovFeature_OrientationControl2D;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feautre.ChrMovAnim.Movement.Walking", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_WalkingMovement : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetStance() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetGait() const;

	/**
	 * Get the normalized axes scale of the character real acceleration relative to the character rotation.
	 * The map represents a manhattan distance of 1.f in all directions.
	 * Value of every direction will be clamped to [0.f, 1.f], based on the real acceleration and LeanMaxAcceleration.
	 * <MovementDirection, LeanAmounts>
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	TMap<FGameplayTag, float> GetLeanAmounts() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetLeanAlpha() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FRotator EvaluateTargetOrientationForInput() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetPossibleInertialBlendingTime() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVSWalkingMovementAnimSettings GetWalkingMovementSettings() const;



	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanBreakFromStartToCycle() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanEnterPivotState() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanRePivot() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanBreakFromPivotCycle() const;

	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimSequence* CalcIdleAnim() const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimSequence* CalcStanceTransitionAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimSequence* CalcStartAnim() const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimSequence* CalcCycleAnim() const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimSequence* CalcPivotAnim() const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimSequence* CalcStopAnim() const;


	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateIdleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
		
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupStanceTransitionAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateStanceTransitionAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupStopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateStopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION( BlueprintCallable, Category = "Node Events")
	void SetupPivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdatePivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

protected:
	virtual void Initialize_Implementation() override;
	virtual void UpdateAnimationThreadSafe_Implementation(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (RowType = "/Script/VSMovementAnim.VSWalkingMovementAnimSettings"))
	FDataTableRowHandle AnimSettingsRow;

	/** Distance curve name for distance matching. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName DistanceCurveName = FName("Distance");

	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FGameplayTag Stance;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FGameplayTag Gait;
	
	/** Whether the stance is changed in this frame. */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bStanceChangedThisFrame = false;
	
	/**
	 * Whether the gait of the current stance is changed in this frame.
	 * If the stance is changed, then this will be true as well.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bGaitChangedThisFrame = false;

	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator EvaluatedTargetOrientationForInput = FRotator::ZeroRotator;

	
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector StartInputRelativeToEvaluated = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector PivotInputRelativeToEvaluated = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector PivotVelocityRelativeToEvaluated = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float StartStrideWarpingAlpha = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float PivotStrideWarpingAlpha = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float PivotStopTime = 0.f;

	
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_WalkingMovement> ChrMovFeature_WalkingMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_OrientationControl2D> ChrMovFeature_OrientationControl2D;

	// UPROPERTY(BlueprintReadOnly, Category = "Reference")
	// TWeakObjectPtr<UVSChrMovFeature_LandRecovery> ChrMovFeature_LandRecovery;

private:
	struct FAnimData
	{
		int32 LastLandRecoveryActionID = 0;
		float LandRecoveryAnimPlayedTime = 0.f;

		FGameplayTag CachedStance;
		FGameplayTag CachedGait;
		FGameplayTag CachedVelocityDirection;

		FVSWalkingMovementAnimSettings* AnimSettingsPtr;
	} AnimData;
};

UCLASS()
class UVSAnimDataAsset_WalkingMovement : public UDataAsset
{
	GENERATED_BODY()

public:
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
