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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static bool IsValidAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static bool AnimSequenceReferenceHasTimeMark(const FVSAnimSequenceReference& AnimSequenceReference, const FName MarkName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static float GetMarkTimeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference, const FName MarkName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static bool AnimSequenceReferenceHasTimePeriod(const FVSAnimSequenceReference& AnimSequenceReference, const FName PeriodName);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static FVector2D GetPeriodTimeRangeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference, const FName PeriodName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim Sequence Reference")
	static FVector2D GetSafePlayTimeRangeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "HasBone", Category = "Anim Assets")
	static bool AnimationHasBone(const UAnimationAsset* Animation, FName BoneName);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "HasCurve", Category = "Anim Assets")
	static bool AnimationHasCurve(const UAnimSequenceBase* Animation, FName CurveName);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetCurveValueAtTime", Category = "Anim Assets")
	static float GetAnimationCurveValueAtTime(const UAnimSequenceBase* Animation, FName CurveName, float Time);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetCurveValueAtFrame", Category = "Anim Assets")
	static float GetAnimationCurveValueAtFrame(const UAnimSequenceBase* Animation, FName CurveName, int32 Frame);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetTimeAtFrame", Category = "Anim Assets")
	static float GetAnimationTimeAtFrame(const UAnimSequenceBase* Animation, const int32 Frame);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetTimeAtFrame", Category = "Anim Assets")
	static int32 GetAnimationFrameAtTime(const UAnimSequenceBase* Animation, float Time);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetBoneTransformAtTime", Category = "Anim Assets")
	static FTransform GetAnimationBoneTransformAtTime(const UAnimSequenceBase* Animation, FName BoneName, float Time, bool bExtractRootMotion = false, bool bUseComponentSpace = true, bool bThreadSafe = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetBoneTransformAtFrame", Category = "Anim Assets")
	static FTransform GetAnimationBoneTransformAtFrame(const UAnimSequenceBase* Animation, FName BoneName, float Frame, bool bExtractRootMotion = false, bool bUseComponentSpace = true, bool bThreadSafe = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "SetInterialBlending", Category = "Anim Node")
	static FSequencePlayerReference SetInterialBlendingForSequencePlayer(const FAnimUpdateContext& UpdateContext, const FSequencePlayerReference& SequencePlayer, float BlendTime = 0.2f);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "SetInterialBlending", Category = "Anim Node")
	static FSequenceEvaluatorReference SetInterialBlendingForSequenceEvaluator(const FAnimUpdateContext& UpdateContext, const FSequenceEvaluatorReference& SequenceEvaluator, float BlendTime = 0.2f);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetLinkedAnimLayers")
	static TArray<UAnimInstance*> GetAnimInstanceLinkedAnimLayers(const UAnimInstance* AnimInstance, bool bRecursive = true);

	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "HasSlot")
	static bool AnimInstanceHasSlot(const UAnimInstance* AnimInstance, FName SlotName);

	/** Get the first anim instance that has the specified slot. Including the passed-in one, and will check linked layers. */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "GetAnimInstanceWithSlot")
	static UAnimInstance* GetAnimInstanceWithSlot(UAnimInstance* AnimInstance, FName SlotName);
	
protected:
	static void GetAnimSequenceBonePose(const UAnimSequence* AnimSequence, FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext, bool bForceUseRawData = false);
};
