// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSAnimationLibrary.generated.h"

struct FVSAnimSequenceReference;
/**
 * Runtime methods for animation assets and nodes.
 */
UCLASS()
class VSPLUGINSCORE_API UVSAnimationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Check if an anim sequence reference is valid. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static bool IsValidAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference);

	/** Check if anim sequence reference contains a time mark. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static bool AnimSequenceReferenceHasTimeMark(const FVSAnimSequenceReference& AnimSequenceReference, const FName MarkName);

	/** Get the time of a specific mark in anim sequence reference. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static float GetMarkTimeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference, const FName MarkName);

	/** Check if anim sequence reference contains a time period. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static bool AnimSequenceReferenceHasTimePeriod(const FVSAnimSequenceReference& AnimSequenceReference, const FName PeriodName);

	/** Get start and end time of a period in anim sequence reference. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static FVector2D GetPeriodTimeRangeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference, const FName PeriodName);

	/** Get safe play time range for anim sequence reference. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static FVector2D GetSafePlayTimeRangeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference);

	/** Check if animation asset has specified bone. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "HasBone", Category = "Anim Assets")
	static bool AnimationHasBone(const UAnimationAsset* Animation, FName BoneName);

	/** Check if animation sequence has specified curve. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "HasCurve", Category = "Anim Assets")
	static bool AnimationHasCurve(const UAnimSequenceBase* Animation, FName CurveName);

	/** Get curve value from animation at given time. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetCurveValueAtTime", Category = "Anim Assets")
	static float GetAnimationCurveValueAtTime(const UAnimSequenceBase* Animation, FName CurveName, float Time);

	/** Get curve value from animation at given frame. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetCurveValueAtFrame", Category = "Anim Assets")
	static float GetAnimationCurveValueAtFrame(const UAnimSequenceBase* Animation, FName CurveName, int32 Frame);

	/** Get animation time corresponding to a given frame. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetTimeAtFrame", Category = "Anim Assets")
	static float GetAnimationTimeAtFrame(const UAnimSequenceBase* Animation, const int32 Frame);

	/** Get animation frame corresponding to a given time. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetTimeAtFrame", Category = "Anim Assets")
	static int32 GetAnimationFrameAtTime(const UAnimSequenceBase* Animation, float Time);

	/**
	 * Get bone transform from animation at given time.
	 * Supports root motion extraction and component / local space.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetBoneTransformAtTime", Category = "Anim Assets")
	static FTransform GetAnimationBoneTransformAtTime(const UAnimSequenceBase* Animation, FName BoneName, float Time, bool bExtractRootMotion = false, bool bUseComponentSpace = true, bool bThreadSafe = false);

	/**
	 * Get bone transform from animation at given frame.
	 * Supports root motion extraction and component / local space.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetBoneTransformAtFrame", Category = "Anim Assets")
	static FTransform GetAnimationBoneTransformAtFrame(const UAnimSequenceBase* Animation, FName BoneName, float Frame, bool bExtractRootMotion = false, bool bUseComponentSpace = true, bool bThreadSafe = false);

	/** Set inertial blending for sequence player node. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "SetInterialBlending", Category = "Anim Node")
	static FSequencePlayerReference SetInterialBlendingForSequencePlayer(const FAnimUpdateContext& UpdateContext, const FSequencePlayerReference& SequencePlayer, float BlendTime = 0.2f);

	/** Set inertial blending for sequence evaluator node. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "SetInterialBlending", Category = "Anim Node")
	static FSequenceEvaluatorReference SetInterialBlendingForSequenceEvaluator(const FAnimUpdateContext& UpdateContext, const FSequenceEvaluatorReference& SequenceEvaluator, float BlendTime = 0.2f);

	/**
	 * Get linked anim layers from an anim instance.
	 * Can be recursive to include all child layers.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetLinkedAnimLayers")
	static TArray<UAnimInstance*> GetAnimInstanceLinkedAnimLayers(const UAnimInstance* AnimInstance, bool bRecursive = true);

	/** Check if anim instance has a slot with given name. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "HasSlot")
	static bool AnimInstanceHasSlot(const UAnimInstance* AnimInstance, FName SlotName);

	/**
	 * Get the first anim instance that has the specified slot.
	 * The path includes the passed-in one, and will check linked layers.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetAnimInstanceWithSlot")
	static UAnimInstance* GetAnimInstanceWithSlot(UAnimInstance* AnimInstance, FName SlotName);
	
protected:
	static void GetAnimSequenceBonePose(const UAnimSequence* AnimSequence, FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext, bool bForceUseRawData = false);
};
