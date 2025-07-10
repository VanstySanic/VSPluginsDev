// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovAnimFeature_FixedPointLeap.h"
#include "SequenceEvaluatorLibrary.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Movement/VSChrMovFeature_FixedPointLeap.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovAnimFeature_FixedPointLeap::UVSChrMovAnimFeature_FixedPointLeap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSChrMovAnimFeature_FixedPointLeap::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ChrMovFeature_FixedPointLeapMovement = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_FixedPointLeap>();
	check(ChrMovFeature_FixedPointLeapMovement.IsValid());
}

bool UVSChrMovAnimFeature_FixedPointLeap::HasValidFixedPointLeapAnim_Implementation() const
{
	FVSAnimSequenceReference* Anim = ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	return Anim && Anim->IsValid();
}

void UVSChrMovAnimFeature_FixedPointLeap::SetupFixedPointLeapAnim_Implementation(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_FixedPointLeapMovement.IsValid() || !HasValidFixedPointLeapAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* Anim = ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (UAnimSequenceBase* NewAnim = Anim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, Anim->GetSafePlayTimeRange().X);
	AnimData.LastActionID = ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().ActionID;
}

void UVSChrMovAnimFeature_FixedPointLeap::UpdateFixedPointLeapAnim_Implementation(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_FixedPointLeapMovement.IsValid() || !HasValidFixedPointLeapAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* Anim = ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	/** Check for new mantle process. */
	if (AnimData.LastActionID != ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().ActionID && ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().ActionID != INDEX_NONE && ChrMovFeature_FixedPointLeapMovement->IsFixedPointLeapMode())
	{
		if (UAnimSequenceBase* NewAnim = Anim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, Anim->GetSafePlayTimeRange().X);
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
		AnimData.LastActionID = ChrMovFeature_FixedPointLeapMovement->GetFixedPointLeapSnappedParams().ActionID;
	}
	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, Anim->PlayRate);
}
