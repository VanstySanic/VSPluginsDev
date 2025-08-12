// Copyright VanstySanic. All Rights Reserved.

#include "Features/Details/VSChrMovAnimFeature_LandRecovery.h"
#include "SequenceEvaluatorLibrary.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Details/VSChrMovFeature_LandRecovery.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovAnimFeature_LandRecovery::UVSChrMovAnimFeature_LandRecovery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovAnimFeature_LandRecovery::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ChrMovFeature_LandRecovery = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_LandRecovery>();
	check(ChrMovFeature_LandRecovery.IsValid());
}

bool UVSChrMovAnimFeature_LandRecovery::HasValidLandRecoveryAnim() const
{
	if (!ChrMovFeature_LandRecovery.IsValid()) return false;

	const FVSLandRecoveryAnimParams& LandRecoveryCachedData = ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams();
	FVSAnimSequenceReference* LandRecoveryAnim = LandRecoveryCachedData.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!LandRecoveryAnim || !LandRecoveryAnim->IsValid()) return false;
	return true;
}

bool UVSChrMovAnimFeature_LandRecovery::CanEnterNonAdditiveLandRecoveryState() const
{
	if (!ChrMovFeature_LandRecovery.IsValid() || !ChrMovFeature_LandRecovery->IsLandRecovering()) return false;
	if (ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams().AnimPlayPolicy == EVSLandRecoveryAnimPlayPolicy::None
		|| ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams().AnimPlayPolicy == EVSLandRecoveryAnimPlayPolicy::PlayAsAdditive) return false;
	if (!HasValidLandRecoveryAnim()) return false;
	
	return true;
}

bool UVSChrMovAnimFeature_LandRecovery::CanLandRecoveryAnimPlayAsAdditive() const
{
	if (!ChrMovFeature_LandRecovery.IsValid() || !ChrMovFeature_LandRecovery->IsLandRecovering()) return false;
	if (ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams().AnimPlayPolicy != EVSLandRecoveryAnimPlayPolicy::PlayAsAdditive) return false;
	
	const FVSLandRecoveryAnimParams& LandRecoveryCachedData = ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams();
	FVSAnimSequenceReference* LandRecoveryAnim = LandRecoveryCachedData.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	
	if (!LandRecoveryAnim || !LandRecoveryAnim->IsValid() || !LandRecoveryAnim->AnimSequence->IsValidAdditive()) return false;
	
	return true;
}

void UVSChrMovAnimFeature_LandRecovery::SetupLandRecoveryAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_LandRecovery.IsValid()) return;
	const FVSLandRecoveryAnimParams& LandRecoveryCachedData = ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams();
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* LandRecoveryAnim = LandRecoveryCachedData.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!LandRecoveryAnim || !LandRecoveryAnim->IsValid()) return;

	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, LandRecoveryAnim->AnimSequence);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, LandRecoveryAnim->HasTimeMark(AnimReachGroundTimeMarkName) ? LandRecoveryAnim->GetMarkTime(AnimReachGroundTimeMarkName) : LandRecoveryAnim->GetSafePlayTimeRange().X);

	AnimData.LastActionID = LandRecoveryCachedData.ActionID;
}

void UVSChrMovAnimFeature_LandRecovery::UpdateLandRecoveryAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_LandRecovery.IsValid()) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;
	
	if (!GetCharacterMovement() || !ChrMovFeature_LandRecovery.IsValid()) return;
	const FVSLandRecoveryAnimParams& LandRecoveryCachedData = ChrMovFeature_LandRecovery->GetLandRecoveryAnimParams();

	FVSAnimSequenceReference* LandRecoveryAnim = LandRecoveryCachedData.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!LandRecoveryAnim || !LandRecoveryAnim->IsValid()) return;
	
	/** New recovery state. */
	if (AnimData.LastActionID != LandRecoveryCachedData.ActionID)
	{
		if (LandRecoveryAnim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, LandRecoveryAnim->AnimSequence);
		}
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, LandRecoveryAnim->HasTimeMark(AnimReachGroundTimeMarkName) ? LandRecoveryAnim->GetMarkTime(AnimReachGroundTimeMarkName) : LandRecoveryAnim->GetSafePlayTimeRange().X);
	}

	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, LandRecoveryAnim->PlayRate);
}

