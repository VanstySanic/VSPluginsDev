// Copyright VanstySanic. All Rights Reserved.


#include "Features/Movement/VSChrMovAnimFeature_MantleVaultMovement.h"
#include "SequenceEvaluatorLibrary.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Movement/VSChrMovFeature_MantleVaultMovement.h"
#include "Libraries/VSAnimationLibrary.h"

UVSChrMovAnimFeature_MantleVaultMovement::UVSChrMovAnimFeature_MantleVaultMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovAnimFeature_MantleVaultMovement::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ChrMovFeature_MantleVaultMovement = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_MantleVaultMovement>();
	check(ChrMovFeature_MantleVaultMovement.IsValid());
}

bool UVSChrMovAnimFeature_MantleVaultMovement::HasValidMantleVaultAnim_Implementation() const
{
	FVSMantleVaultAnimSettings* AnimSettings = ChrMovFeature_MantleVaultMovement->GetMantleVaultSnappedParams().AnimSettingsRow.GetRow<FVSMantleVaultAnimSettings>(nullptr);
	if (!AnimSettings) return false;
	FVSAnimSequenceReference* Anim = AnimSettings->AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	return Anim && Anim->IsValid();
}

void UVSChrMovAnimFeature_MantleVaultMovement::SetupMantleVaultAnim_Implementation(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_MantleVaultMovement.IsValid() || !HasValidMantleVaultAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSMantleVaultAnimSettings* AnimSettings = ChrMovFeature_MantleVaultMovement->GetMantleVaultSnappedParams().AnimSettingsRow.GetRow<FVSMantleVaultAnimSettings>(nullptr);
	if (!AnimSettings) return;
	FVSAnimSequenceReference* Anim = AnimSettings->AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!Anim) return;

	if (UAnimSequenceBase* NewAnim = Anim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, Anim->GetSafePlayTimeRange().X);
	AnimData.LastActionID = ChrMovFeature_MantleVaultMovement->GetMantleVaultCachedParams().ActionID;
}

void UVSChrMovAnimFeature_MantleVaultMovement::UpdateMantleVaultAnim_Implementation(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_MantleVaultMovement.IsValid() || !HasValidMantleVaultAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSMantleVaultAnimSettings* AnimSettings = ChrMovFeature_MantleVaultMovement->GetMantleVaultSnappedParams().AnimSettingsRow.GetRow<FVSMantleVaultAnimSettings>(nullptr);
	if (!AnimSettings) return;
	FVSAnimSequenceReference* Anim = AnimSettings->AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!Anim) return;

	/** Check for new mantle process. */
	if (AnimData.LastActionID != ChrMovFeature_MantleVaultMovement->GetMantleVaultCachedParams().ActionID && ChrMovFeature_MantleVaultMovement->GetMantleVaultCachedParams().ActionID != INDEX_NONE && ChrMovFeature_MantleVaultMovement->IsMantlingOrVaultingMode())
	{
		if (UAnimSequenceBase* NewAnim = Anim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, Anim->GetSafePlayTimeRange().X);
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
		AnimData.LastActionID = ChrMovFeature_MantleVaultMovement->GetMantleVaultCachedParams().ActionID;
	}
	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, ChrMovFeature_MantleVaultMovement->GetMantleVaultCachedParams().AnimPlayRate);
}
