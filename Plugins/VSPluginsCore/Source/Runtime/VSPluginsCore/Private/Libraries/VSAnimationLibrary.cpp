// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSAnimationLibrary.h"
#include "Animation/AnimNode_Inertialization.h"
#include "Animation/AnimNode_LinkedAnimLayer.h"
#include "Animation/AnimSequenceDecompressionContext.h"
#include "Animation/SkeletonRemappingRegistry.h"
#include "AnimNodes/AnimNode_SequenceEvaluator.h"
#include "AnimNodes/AnimNode_Slot.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSAnimationLibrary::UVSAnimationLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSAnimationLibrary::IsValidAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference)
{
	return AnimSequenceReference.IsValid();
}

bool UVSAnimationLibrary::AnimSequenceReferenceHasTimeMark(const FVSAnimSequenceReference& AnimSequenceReference, const FName MarkName)
{
	return AnimSequenceReference.HasTimeMark(MarkName);
}

float UVSAnimationLibrary::GetMarkTimeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference, const FName MarkName)
{
	return AnimSequenceReference.GetMarkTime(MarkName);
}

bool UVSAnimationLibrary::AnimSequenceReferenceHasTimePeriod(const FVSAnimSequenceReference& AnimSequenceReference, const FName PeriodName)
{
	return AnimSequenceReference.HasTimePeriod(PeriodName);
}

FVector2D UVSAnimationLibrary::GetPeriodTimeRangeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference, const FName PeriodName)
{
	return AnimSequenceReference.GetPeriodTimeRange(PeriodName);
}

FVector2D UVSAnimationLibrary::GetSafePlayTimeRangeForAnimSequenceReference(const FVSAnimSequenceReference& AnimSequenceReference)
{
	return AnimSequenceReference.GetSafePlayTimeRange();
}

bool UVSAnimationLibrary::AnimationHasBone(const UAnimationAsset* Animation, FName BoneName)
{
	if (!Animation) return false;
	USkeleton* Skeleton = Animation->GetSkeleton();
	if (!Skeleton) return false;

	const int32 RootBoneIndex = Skeleton->GetReferenceSkeleton().FindBoneIndex(BoneName);
	return RootBoneIndex != INDEX_NONE;
}

bool UVSAnimationLibrary::AnimationHasCurve(const UAnimSequenceBase* Animation, FName CurveName)
{
	if (!Animation) return false;
	return Animation->HasCurveData(CurveName);
}

float UVSAnimationLibrary::GetAnimationCurveValueAtTime(const UAnimSequenceBase* Animation, FName CurveName, float Time)
{
	if (!AnimationHasCurve(Animation, CurveName)) return 0.f;
	Time = FMath::Clamp(Time, 0.f, Animation->GetPlayLength());
	return Animation->EvaluateCurveData(CurveName, Time);
}

float UVSAnimationLibrary::GetAnimationCurveValueAtFrame(const UAnimSequenceBase* Animation, FName CurveName, int32 Frame)
{
	if (!AnimationHasCurve(Animation, CurveName)) return 0.f;
	Frame = FMath::Clamp(Frame, 0, Animation->GetNumberOfSampledKeys());
	const float Time = GetAnimationTimeAtFrame(Animation, Frame);
	
	return Animation->EvaluateCurveData(CurveName, Time);
}

float UVSAnimationLibrary::GetAnimationTimeAtFrame(const UAnimSequenceBase* Animation, const int32 Frame)
{
	if (!Animation) return 0.f;
	return FMath::Clamp((float)Animation->GetSamplingFrameRate().AsSeconds(Frame), 0.f, Animation->GetPlayLength());;
}

int32 UVSAnimationLibrary::GetAnimationFrameAtTime(const UAnimSequenceBase* Animation, float Time)
{
	if (!Animation) return 0.f;
	return FMath::Clamp(Animation->GetSamplingFrameRate().AsFrameTime(Time).RoundToFrame().Value, 0, Animation->GetNumberOfSampledKeys() - 1);
}

FTransform UVSAnimationLibrary::GetAnimationBoneTransformAtTime(const UAnimSequenceBase* Animation, FName BoneName, float Time, bool bExtractRootMotion, bool bUseComponentSpace, bool bThreadSafe)
{
	if (Animation == nullptr) return FTransform::Identity;
	const UAnimSequenceBase* AnimationToCheck = bThreadSafe ? DuplicateObject(Animation, nullptr) : Animation;
	
	USkeleton* Skeleton = AnimationToCheck->GetSkeleton();
	if (Skeleton == nullptr) return FTransform::Identity;
	
	const int32 BoneIndex = Skeleton->GetReferenceSkeleton().FindBoneIndex(BoneName);
	if (BoneIndex == INDEX_NONE) return FTransform::Identity;
	
	TArray<FBoneIndexType> RequiredBones;
	RequiredBones.Add(IntCastChecked<FBoneIndexType>(BoneIndex));
	
	Skeleton->GetReferenceSkeleton().EnsureParentsExistAndSort(RequiredBones);
	FBoneContainer BoneContainer(RequiredBones, UE::Anim::ECurveFilterMode::DisallowAll, *Skeleton);
	const FCompactPoseBoneIndex CompactPoseBoneIndex = BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(BoneIndex));

	
	FCompactPose Pose;
	Pose.SetBoneContainer(&BoneContainer);

	FBlendedCurve Curve;
	Curve.InitFrom(BoneContainer);

	FAnimExtractContext Context(static_cast<double>(Time), bExtractRootMotion);
	UE::Anim::FStackAttributeContainer Attributes;
	FAnimationPoseData AnimationToCheckPoseData(Pose, Curve, Attributes);

	if (const UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimationToCheck))
	{
		//TGuardValue<bool> ForceRootLockGuard = TGuardValue<bool>(AnimSequence->bForceRootLock, false);
		GetAnimSequenceBonePose(AnimSequence, AnimationToCheckPoseData, Context);
	}
	else
	{
		AnimationToCheck->GetAnimationPose(AnimationToCheckPoseData, Context);
	}

	check(Pose.IsValidIndex(CompactPoseBoneIndex));
	
	if (bUseComponentSpace)
	{
		FCSPose<FCompactPose> ComponentSpacePose;
		ComponentSpacePose.InitPose(Pose);
		return ComponentSpacePose.GetComponentSpaceTransform(CompactPoseBoneIndex);
	}

	if (bThreadSafe)
	{
		const_cast<UAnimSequenceBase*>(AnimationToCheck)->MarkAsGarbage();
	}

	return Pose[CompactPoseBoneIndex];
}

FTransform UVSAnimationLibrary::GetAnimationBoneTransformAtFrame(const UAnimSequenceBase* Animation, FName BoneName, float Frame, bool bExtractRootMotion, bool bUseComponentSpace, bool bThreadSafe)
{
	if (Animation == nullptr) return FTransform::Identity;
	const float Time = GetAnimationTimeAtFrame(Animation, Frame);
	return GetAnimationBoneTransformAtTime(Animation, BoneName, Time, bExtractRootMotion, bUseComponentSpace, bThreadSafe);
}

FSequencePlayerReference UVSAnimationLibrary::SetInterialBlendingForSequencePlayer(const FAnimUpdateContext& UpdateContext, const FSequencePlayerReference& SequencePlayer, float BlendTime)
{
	SequencePlayer.CallAnimNodeFunction<FAnimNode_SequenceEvaluator>(
	TEXT("SetInterialBlending"),
	[&UpdateContext, BlendTime](FAnimNode_SequenceEvaluator& InSequenceEvaluator)
	{
		if(BlendTime > 0.0f)
		{
			if (const FAnimationUpdateContext* AnimationUpdateContext = UpdateContext.GetContext())
			{
				if (UE::Anim::IInertializationRequester* InertializationRequester = AnimationUpdateContext->GetMessage<UE::Anim::IInertializationRequester>())
				{
					FInertializationRequest Request;
					Request.Duration = BlendTime;

					InertializationRequester->RequestInertialization(Request);
				}
			}
		}
	});

	return SequencePlayer;
}


FSequenceEvaluatorReference UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(const FAnimUpdateContext& UpdateContext, const FSequenceEvaluatorReference& SequenceEvaluator, float BlendTime)
{
	SequenceEvaluator.CallAnimNodeFunction<FAnimNode_SequenceEvaluator>(
	TEXT("SetInterialBlending"),
	[&UpdateContext, BlendTime](FAnimNode_SequenceEvaluator& InSequenceEvaluator)
	{
		if(BlendTime > 0.0f)
		{
			if (const FAnimationUpdateContext* AnimationUpdateContext = UpdateContext.GetContext())
			{
				if (UE::Anim::IInertializationRequester* InertializationRequester = AnimationUpdateContext->GetMessage<UE::Anim::IInertializationRequester>())
				{
					FInertializationRequest Request;
					Request.Duration = BlendTime;

					InertializationRequester->RequestInertialization(Request);
				}
			}
		}
	});

	return SequenceEvaluator;
}

TArray<UAnimInstance*> UVSAnimationLibrary::GetAnimInstanceLinkedAnimLayers(const UAnimInstance* AnimInstance, bool bRecursive)
{
	if (!AnimInstance) return TArray<UAnimInstance*>();
	TArray<UAnimInstance*> AnimInstances;
	if (IAnimClassInterface* AnimBlueprintClass = IAnimClassInterface::GetFromClass(AnimInstance->GetClass()))
	{
		for(const FStructProperty* LayerNodeProperty : AnimBlueprintClass->GetLinkedAnimLayerNodeProperties())
		{
			const FAnimNode_LinkedAnimLayer* Layer = LayerNodeProperty->ContainerPtrToValuePtr<FAnimNode_LinkedAnimLayer>(AnimInstance);
			if (!Layer) continue;
			if (UAnimInstance* TargetInstance = Layer->GetTargetInstance<UAnimInstance>())
			{
				AnimInstances.Add(TargetInstance);
				if (bRecursive)
				{
					AnimInstances.Append(GetAnimInstanceLinkedAnimLayers(TargetInstance, true));
				}
			}
		}
	}

	return AnimInstances;
}

bool UVSAnimationLibrary::AnimInstanceHasSlot(const UAnimInstance* AnimInstance, FName SlotName)
{
	if (!AnimInstance) return false;
	if (IAnimClassInterface* AnimBlueprintClass = IAnimClassInterface::GetFromClass(AnimInstance->GetClass()))
	{
		
		for(const FStructProperty* LayerNodeProperty : AnimBlueprintClass->GetAnimNodeProperties())
		{
			const FAnimNode_Slot* Slot = LayerNodeProperty->ContainerPtrToValuePtr<FAnimNode_Slot>(AnimInstance);
			if (Slot && Slot->SlotName == SlotName)
			{
				return true;
			}
		}
	}
	return false;
}

UAnimInstance* UVSAnimationLibrary::GetAnimInstanceWithSlot(UAnimInstance* AnimInstance, FName SlotName)
{
	if (!AnimInstance) return nullptr;
	if (IAnimClassInterface* AnimBlueprintClass = IAnimClassInterface::GetFromClass(AnimInstance->GetClass()))
	{
		for(const FStructProperty* LayerNodeProperty : AnimBlueprintClass->GetAnimNodeProperties())
		{
			const FAnimNode_Slot* Slot = LayerNodeProperty->ContainerPtrToValuePtr<FAnimNode_Slot>(AnimInstance);
			if (Slot && Slot->SlotName == SlotName)
			{
				return AnimInstance;
			}
		}

		for(const FStructProperty* LayerNodeProperty : AnimBlueprintClass->GetLinkedAnimLayerNodeProperties())
		{
			const FAnimNode_LinkedAnimLayer* Layer = LayerNodeProperty->ContainerPtrToValuePtr<FAnimNode_LinkedAnimLayer>(AnimInstance);
			if (!Layer) continue;
			if (UAnimInstance* TargetInstance = Layer->GetTargetInstance<UAnimInstance>())
			{
				if (UAnimInstance* FoundInstance = GetAnimInstanceWithSlot(TargetInstance, SlotName))
				{
					return FoundInstance;
				}
			}
		}
	}

	return nullptr;
}

void UVSAnimationLibrary::GetAnimSequenceBonePose(const UAnimSequence* AnimSequence, FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext, bool bForceUseRawData)
{
	if (!AnimSequence) return;
	FCompactPose& OutPose = OutAnimationPoseData.GetPose();

	FBoneContainer& RequiredBones = OutPose.GetBoneContainer();
#if WITH_EDITORONLY_DATA
	check(!bForceUseRawData || AnimSequence->IsDataModelValid());
	const bool bUseRawDataForPoseExtraction = (AnimSequence->IsDataModelValid() && bForceUseRawData) || AnimSequence->UseRawDataForPoseExtraction(RequiredBones);
#else
	const bool bUseRawDataForPoseExtraction = false;
#endif
	const bool bIsBakedAdditive = !bUseRawDataForPoseExtraction && AnimSequence->IsValidAdditive();

	const USkeleton* MySkeleton = AnimSequence->GetSkeleton();
	if (!MySkeleton)
	{
		if (bIsBakedAdditive)
		{
			OutPose.ResetToAdditiveIdentity();
		}
		else
		{
			OutPose.ResetToRefPose();
		}
		return;
	}

	const bool bDisableRetargeting = RequiredBones.GetDisableRetargeting();

	// initialize with ref-pose
	if (bIsBakedAdditive)
	{
		//When using baked additive ref pose is identity
		OutPose.ResetToAdditiveIdentity();
	}
	else
	{
		// if retargeting is disabled, we initialize pose with 'Retargeting Source' ref pose.
		if (bDisableRetargeting)
		{
			TArray<FTransform> const& AuthoredOnRefSkeleton = AnimSequence->GetRetargetTransforms();
			// Map from this sequence its Skeleton to target
			const FSkeletonRemapping& SkeletonRemapping = UE::Anim::FSkeletonRemappingRegistry::Get().GetRemapping(MySkeleton, RequiredBones.GetSkeletonAsset());
			for (FCompactPoseBoneIndex PoseBoneIndex : OutPose.ForEachBoneIndex())
			{
				int32 SkeletonBoneIndex = RequiredBones.GetSkeletonPoseIndexFromCompactPoseIndex(PoseBoneIndex).GetInt();
				SkeletonBoneIndex = SkeletonRemapping.IsValid() ? SkeletonRemapping.GetSourceSkeletonBoneIndex(SkeletonBoneIndex) : SkeletonBoneIndex;

				// Virtual bones are part of the retarget transform pose, so if the pose has not been updated (recently) there might be a mismatch
				if (SkeletonBoneIndex != INDEX_NONE && AuthoredOnRefSkeleton.IsValidIndex(SkeletonBoneIndex))
				{
					OutPose[PoseBoneIndex] = AuthoredOnRefSkeleton[SkeletonBoneIndex];
				}
			}
		}
		else
		{
			OutPose.ResetToRefPose();
		}
	}

#if WITH_EDITOR
	const int32 NumTracks = bUseRawDataForPoseExtraction ? AnimSequence->GetDataModelInterface()->GetNumBoneTracks() : AnimSequence->CompressedData.CompressedTrackToSkeletonMapTable.Num();
#else
	const int32 NumTracks = AnimSequence->CompressedData.CompressedTrackToSkeletonMapTable.Num();
#endif 
	const bool bTreatAnimAsAdditive = (AnimSequence->IsValidAdditive() && !bUseRawDataForPoseExtraction); // Raw data is never additive
	const FRootMotionReset RootMotionReset(AnimSequence->bEnableRootMotion, AnimSequence->RootMotionRootLock,
#if WITH_EDITOR
		//!ExtractionContext.bIgnoreRootLock &&
#endif // WITH_EDITOR
		//AnimSequence->bForceRootLock,
		ExtractionContext.bExtractRootMotion,
		AnimSequence->ExtractRootTrackTransform(0.0f, &RequiredBones), bTreatAnimAsAdditive);

#if WITH_EDITOR
	// Evaluate raw (source) curve and bone data
	if (bUseRawDataForPoseExtraction)
	{
		{
			const EAnimInterpolationType InterpolationType = ExtractionContext.InterpolationOverride.Get(AnimSequence->Interpolation);
			const UE::Anim::DataModel::FEvaluationContext EvaluationContext(ExtractionContext.CurrentTime, AnimSequence->GetDataModelInterface()->GetFrameRate(), AnimSequence->GetRetargetTransformsSourceName(), AnimSequence->GetRetargetTransforms(), InterpolationType);
			AnimSequence->GetDataModelInterface()->Evaluate(OutAnimationPoseData, EvaluationContext);
		}

		if (ExtractionContext.bExtractRootMotion /*(ExtractionContext.bExtractRootMotion && RootMotionReset.bEnableRootMotion) || RootMotionReset.bForceRootLock*/)
		{
			RootMotionReset.ResetRootBoneForRootMotion(OutPose[FCompactPoseBoneIndex(0)], RequiredBones);
		}

		return;
	}
	else
#endif // WITH_EDITOR
	// Only try and evaluate compressed bone data if the animation contains any bone tracks
	if (NumTracks != 0)
	{
		// Evaluate compressed bone data
		const EAnimInterpolationType InterpolationType = ExtractionContext.InterpolationOverride.Get(AnimSequence->Interpolation);
		FAnimSequenceDecompressionContext DecompContext(AnimSequence->GetSamplingFrameRate(), AnimSequence->GetSamplingFrameRate().AsFrameTime(AnimSequence->GetPlayLength()).RoundToFrame().Value, InterpolationType, AnimSequence->GetRetargetTransformsSourceName(), *AnimSequence->CompressedData.CompressedDataStructure, AnimSequence->GetSkeleton()->GetRefLocalPoses(), AnimSequence->CompressedData.CompressedTrackToSkeletonMapTable, AnimSequence->GetSkeleton(), AnimSequence->IsValidAdditive(), AnimSequence->AdditiveAnimType);
		UE::Anim::Decompression::DecompressPose(OutPose, AnimSequence->CompressedData, ExtractionContext, DecompContext, AnimSequence->GetRetargetTransforms(), RootMotionReset);
	}

	// (Always) evaluate compressed curve data
	{
#if WITH_EDITOR
		// When evaluating from raw animation data, UE::Anim::BuildPoseFromModel will populate the curve data
		if (!bUseRawDataForPoseExtraction)
#endif // WITH_EDITOR
		{
			AnimSequence->EvaluateCurveData(OutAnimationPoseData.GetCurve(), ExtractionContext.CurrentTime, bUseRawDataForPoseExtraction);
		}
	}

	// Evaluate animation attributes (no compressed format yet)
	AnimSequence->EvaluateAttributes(OutAnimationPoseData, ExtractionContext, false);
}
