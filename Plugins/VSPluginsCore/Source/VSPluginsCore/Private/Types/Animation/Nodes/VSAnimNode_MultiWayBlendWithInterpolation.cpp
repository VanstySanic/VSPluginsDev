// Copyright VanstySanic. All Rights Reserved.

#include "Types/Animation/Nodes/VSAnimNode_MultiWayBlendWithInterpolation.h"

struct FVSPC_MultiBlendData : public TThreadSingleton<FVSPC_MultiBlendData>
{
	TArray<FCompactPose, TInlineAllocator<8>> SourcePoses;
	TArray<float, TInlineAllocator<8>> SourceWeights;
	TArray<FBlendedCurve, TInlineAllocator<8>> SourceCurves;
	TArray<UE::Anim::FStackAttributeContainer, TInlineAllocator<8>> SourceAttributes;
};

void FVSAnimNode_MultiWayBlendWithInterpolation::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(Context);

	if (!ensure(Poses.Num() == DesiredAlphas.Num()))
	{
		DesiredAlphas.Init(0.f, Poses.Num());
		CachedAlphas.Init(0.f, Poses.Num());
	}

	//UpdateCachedAlphas(0.f, 0.f);
	CachedAlphas = DesiredAlphas;

	for (FPoseLink& Pose : Poses)
	{
		Pose.Initialize(Context);
	}
}

void FVSAnimNode_MultiWayBlendWithInterpolation::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
	for (FPoseLink& Pose : Poses)
	{
		Pose.CacheBones(Context);
	}
}

void FVSAnimNode_MultiWayBlendWithInterpolation::UpdateCachedAlphas(float DeltaTime, float InInterpSpeed)
{
	float TotalAlpha = GetTotalAlpha();

	const int32 DesiredAlphaNums = DesiredAlphas.Num();
	if (DesiredAlphaNums > 0)
	{
		if (DesiredAlphaNums > CachedAlphas.Num())
		{
			int32 DeltaCount = DesiredAlphaNums - CachedAlphas.Num();
			for (int i = DeltaCount; i > 0; i--)
			{
				CachedAlphas.Push(DesiredAlphas[DesiredAlphaNums - i]);
			}
		}
		else if (DesiredAlphaNums < CachedAlphas.Num())
		{
			while (DesiredAlphaNums < CachedAlphas.Num())
			{
				CachedAlphas.Pop();
			}
		}
	}

	const float ActualTotalAlpha = AlphaScaleBias.ApplyTo(TotalAlpha);
	if (ActualTotalAlpha > ZERO_ANIMWEIGHT_THRESH)
	{
		// if (bNormalizeAlpha)
		// {
		// 	for (int32 AlphaIndex = 0; AlphaIndex < DesiredAlphas.Num(); ++AlphaIndex)
		// 	{
		// 		const float DesiredAlpha = AlphaScaleBias.ApplyTo(DesiredAlphas[AlphaIndex] / TotalAlpha);
		// 		CachedAlphas[AlphaIndex] = bShouldInterp ? FMath::FInterpTo(CachedAlphas[AlphaIndex], DesiredAlpha, DeltaTime, InterpSpeed) : DesiredAlpha;
		// 	}
		// }
		// else
		// {
		// 	for (int32 AlphaIndex = 0; AlphaIndex < DesiredAlphas.Num(); ++AlphaIndex)
		// 	{
		// 		const float DesiredAlpha = AlphaScaleBias.ApplyTo(DesiredAlphas[AlphaIndex]);
		// 		CachedAlphas[AlphaIndex] = bShouldInterp ? FMath::FInterpTo(CachedAlphas[AlphaIndex], DesiredAlpha, DeltaTime, InterpSpeed) : DesiredAlpha;
		// 	}
		// }
		const bool bShouldInterp = DeltaTime > UE_KINDA_SMALL_NUMBER && InInterpSpeed > 0.f;
		for (int32 AlphaIndex = 0; AlphaIndex < DesiredAlphas.Num(); ++AlphaIndex)
		{
			const float DesiredAlpha = AlphaScaleBias.ApplyTo(DesiredAlphas[AlphaIndex]);
			CachedAlphas[AlphaIndex] = bShouldInterp ? FMath::FInterpTo(CachedAlphas[AlphaIndex], DesiredAlpha, DeltaTime, InterpSpeed) : DesiredAlpha;
		}
	}

	ensure(Poses.Num() == CachedAlphas.Num());
}

void FVSAnimNode_MultiWayBlendWithInterpolation::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FAnimationNode_MultiWayBlend_Update);
	GetEvaluateGraphExposedInputs().Execute(Context);
	UpdateCachedAlphas(Context.GetDeltaTime(), InterpSpeed);

	if (bNormalizeAlpha)
	{
		float TotalAlpha = GetTotalCachedAlpha();
		const float ActualTotalAlpha = AlphaScaleBias.ApplyTo(TotalAlpha);
		if (ActualTotalAlpha > ZERO_ANIMWEIGHT_THRESH)
		{
			for (int32 PoseIndex = 0; PoseIndex < Poses.Num(); ++PoseIndex)
			{
				float CurrentAlpha = CachedAlphas[PoseIndex] / TotalAlpha;
				if (CurrentAlpha > ZERO_ANIMWEIGHT_THRESH)
				{
					Poses[PoseIndex].Update(Context.FractionalWeight(CurrentAlpha));
				}
			}
		}
	}
	else
	{
		for (int32 PoseIndex = 0; PoseIndex < Poses.Num(); ++PoseIndex)
		{
			float CurrentAlpha = CachedAlphas[PoseIndex];
			if (CurrentAlpha > ZERO_ANIMWEIGHT_THRESH)
			{
				Poses[PoseIndex].Update(Context.FractionalWeight(CurrentAlpha));
			}
		}
	}
}

void FVSAnimNode_MultiWayBlendWithInterpolation::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(MultiWayBlend, !IsInGameThread());
	
	FVSPC_MultiBlendData& BlendData = FVSPC_MultiBlendData::Get();
	TArray<FCompactPose, TInlineAllocator<8>>& SourcePoses = BlendData.SourcePoses;
	TArray<FBlendedCurve, TInlineAllocator<8>>& SourceCurves = BlendData.SourceCurves;
	TArray<UE::Anim::FStackAttributeContainer, TInlineAllocator<8>>& SourceAttributes = BlendData.SourceAttributes;
	TArray<float, TInlineAllocator<8>>& SourceWeights = BlendData.SourceWeights;

	const int32 SourcePosesInitialNum = SourcePoses.Num();
	int32 SourcePosesAdded = 0;
	if (ensure(Poses.Num() == CachedAlphas.Num()))
	{
		if (bNormalizeAlpha)
		{
			float TotalAlpha = GetTotalCachedAlpha();
			const float ActualTotalAlpha = AlphaScaleBias.ApplyTo(TotalAlpha);
			if (ActualTotalAlpha > ZERO_ANIMWEIGHT_THRESH)
			{
				for (int32 PoseIndex = 0; PoseIndex < Poses.Num(); ++PoseIndex)
				{
					const float CurrentAlpha = CachedAlphas[PoseIndex] / TotalAlpha;
					if (CurrentAlpha > ZERO_ANIMWEIGHT_THRESH)
					{
						FPoseContext PoseContext(Output);
						Poses[PoseIndex].Evaluate(PoseContext);

						FCompactPose& SourcePose = SourcePoses.AddDefaulted_GetRef();
						SourcePose.MoveBonesFrom(PoseContext.Pose);

						FBlendedCurve& SourceCurve = SourceCurves.AddDefaulted_GetRef();
						SourceCurve.MoveFrom(PoseContext.Curve);

						UE::Anim::FStackAttributeContainer& SourceAttribute = SourceAttributes.AddDefaulted_GetRef();
						SourceAttribute.MoveFrom(PoseContext.CustomAttributes);

						SourceWeights.Add(CurrentAlpha);

						++SourcePosesAdded;
					}
				}
			}
		}
		else
		{
			for (int32 PoseIndex = 0; PoseIndex < Poses.Num(); ++PoseIndex)
			{
				const float CurrentAlpha = CachedAlphas[PoseIndex];
				if (CurrentAlpha > ZERO_ANIMWEIGHT_THRESH)
				{
					FPoseContext PoseContext(Output);
					Poses[PoseIndex].Evaluate(PoseContext);

					FCompactPose& SourcePose = SourcePoses.AddDefaulted_GetRef();
					SourcePose.MoveBonesFrom(PoseContext.Pose);

					FBlendedCurve& SourceCurve = SourceCurves.AddDefaulted_GetRef();
					SourceCurve.MoveFrom(PoseContext.Curve);

					UE::Anim::FStackAttributeContainer& SourceAttribute = SourceAttributes.AddDefaulted_GetRef();
					SourceAttribute.MoveFrom(PoseContext.CustomAttributes);

					SourceWeights.Add(CurrentAlpha);

					++SourcePosesAdded;
				}
			}
		}
	}

	if (SourcePosesAdded > 0)
	{
		TArrayView<FCompactPose> SourcePosesView = MakeArrayView(&SourcePoses[SourcePosesInitialNum], SourcePosesAdded);
		TArrayView<FBlendedCurve> SourceCurvesView = MakeArrayView(&SourceCurves[SourcePosesInitialNum], SourcePosesAdded);
		TArrayView<UE::Anim::FStackAttributeContainer> SourceAttributesView = MakeArrayView(&SourceAttributes[SourcePosesInitialNum], SourcePosesAdded);
		TArrayView<float> SourceWeightsView = MakeArrayView(&SourceWeights[SourcePosesInitialNum], SourcePosesAdded);

		FAnimationPoseData AnimationPoseData(Output);

		FAnimationRuntime::BlendPosesTogether(SourcePosesView, SourceCurvesView, SourceAttributesView, SourceWeightsView, AnimationPoseData);
		
		Output.Pose.NormalizeRotations();
		
		SourcePoses.SetNum(SourcePosesInitialNum, EAllowShrinking::No);
		SourceCurves.SetNum(SourcePosesInitialNum, EAllowShrinking::No);
		SourceWeights.SetNum(SourcePosesInitialNum, EAllowShrinking::No);
		SourceAttributes.SetNum(SourcePosesInitialNum, EAllowShrinking::No);
	}
	else
	{
		if (bAdditiveNode)
		{
			Output.ResetToAdditiveIdentity();
		}
		else
		{
			Output.ResetToRefPose();
		}
	}
}

void FVSAnimNode_MultiWayBlendWithInterpolation::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	FString DebugLine = DebugData.GetNodeName(this);
	DebugData.AddDebugItem(DebugLine);

	for (int32 PoseIndex = 0; PoseIndex <Poses.Num(); ++PoseIndex)
	{
		Poses[PoseIndex].GatherDebugData(DebugData.BranchFlow(CachedAlphas[PoseIndex]));
	}
}