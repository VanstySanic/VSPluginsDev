// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/InputScaleBias.h"
#include "VSAnimNode_MultiWayBlendWithInterpolation.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct VSANIMGRAPH_API FVSAnimNode_MultiWayBlendWithInterpolation : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	FVSAnimNode_MultiWayBlendWithInterpolation()
		: bAdditiveNode(false)
		, bNormalizeAlpha(true)
	{
	}

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	int32 AddPose()
	{
		Poses.AddDefaulted();
		DesiredAlphas.AddZeroed();
		UpdateCachedAlphas(0.f, 0.f);

		return Poses.Num();
	}

	void RemovePose(int32 PoseIndex)
	{
		Poses.RemoveAt(PoseIndex);
		CachedAlphas.RemoveAt(PoseIndex);
		DesiredAlphas.RemoveAt(PoseIndex);
	}

	void ResetPoses()
	{
		Poses.Reset();
		CachedAlphas.Reset();
		DesiredAlphas.Reset();
	}

	float GetTotalAlpha() const
	{
		float TotalAlpha = 0.f;

		for (float Alpha : DesiredAlphas)
		{
			TotalAlpha += Alpha;
		}

		return TotalAlpha;
	}

	float GetTotalCachedAlpha() const
	{
		float TotalAlpha = 0.f;

		for (float Alpha : CachedAlphas)
		{
			TotalAlpha += Alpha;
		}

		return TotalAlpha;
	}

protected:
	virtual void UpdateCachedAlphas(float DeltaTime, float InInterpSpeed);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (BlueprintCompilerGeneratedDefaults))
	TArray<FPoseLink> Poses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", EditFixedSize, meta = (BlueprintCompilerGeneratedDefaults, PinShownByDefault))
	TArray<float> DesiredAlphas;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FInputScaleBias AlphaScaleBias;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAdditiveNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bNormalizeAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (BlueprintCompilerGeneratedDefaults))
	float InterpSpeed = 10.f;

protected:
	TArray<float> CachedAlphas;
};
