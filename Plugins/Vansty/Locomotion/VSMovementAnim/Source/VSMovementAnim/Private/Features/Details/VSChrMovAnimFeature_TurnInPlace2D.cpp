// Copyright VanstySanic. All Rights Reserved.

#include "Features/Details/VSChrMovAnimFeature_TurnInPlace2D.h"
#include "SequenceEvaluatorLibrary.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Orientation/VSChrMovFeature_TurnInPlace2D.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovAnimFeature_TurnInPlace2D::UVSChrMovAnimFeature_TurnInPlace2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovAnimFeature_TurnInPlace2D::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ChrMovFeature_TurnInPlace2D = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_TurnInPlace2D>();
	check(ChrMovFeature_TurnInPlace2D.IsValid());
}

bool UVSChrMovAnimFeature_TurnInPlace2D::HasValidTurnInPlaceAnim() const
{
	FVSAnimSequenceReference* Anim = ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().AnimRow.GetRow<FVSAnimSequenceReference>();
	return Anim && Anim->IsValid();
}

void UVSChrMovAnimFeature_TurnInPlace2D::SetupTurnInPlaceAnim_Implementation(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_TurnInPlace2D.IsValid()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* Anim = ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	if (UAnimSequenceBase* NewAnim = Anim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, Anim ? Anim->GetSafePlayTimeRange().X : 0.f);
	AnimData.LastActionID = ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().ActionID;
}

void UVSChrMovAnimFeature_TurnInPlace2D::UpdateTurnInPlaceAnim_Implementation(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_TurnInPlace2D.IsValid()) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* Anim = ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	
	/** New turn in place process. */
	if (AnimData.LastActionID != ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().ActionID && ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().ActionID != INDEX_NONE)
	{
		if (UAnimSequenceBase* NewAnim = Anim ? Anim->AnimSequence : nullptr)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, Anim ? Anim->GetSafePlayTimeRange().X : 0.f);
		AnimData.LastActionID = ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().ActionID;
	}

	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, ChrMovFeature_TurnInPlace2D->GetTurnInPlaceSnappedParams().PlayRateMultiplier * (Anim ? Anim->PlayRate : 1.f));
}
