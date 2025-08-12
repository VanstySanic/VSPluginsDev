// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovAnimFeature_WallRunMovement.h"
#include "AnimDistanceMatchingLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Movement/VSChrMovFeature_WallRunMovement.h"
#include "GameFramework/Character.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovAnimFeature_WallRunMovement::UVSChrMovAnimFeature_WallRunMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovAnimFeature_WallRunMovement::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ChrMovFeature_WallRunMovement = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_WallRunMovement>();
	check(ChrMovFeature_WallRunMovement.IsValid());
}

bool UVSChrMovAnimFeature_WallRunMovement::HasValidWallRunStartAnim() const
{
	return GetStartAnimSequenceReferencePtr() != nullptr;
}

bool UVSChrMovAnimFeature_WallRunMovement::HasValidWallRunEndAnim() const
{
	return GetEndAnimSequenceReferencePtr() != nullptr;
}

FVSAnimSequenceReference* UVSChrMovAnimFeature_WallRunMovement::GetStartAnimSequenceReferencePtr() const
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || ChrMovFeature_WallRunMovement->GetWallRunState() != EVSWallRunState::Starting) return nullptr;
	const FVSWallRunSnappedParams& SnappedParams = ChrMovFeature_WallRunMovement->GetWallRunSnappedParams();
	if (SnappedParams.SettingsRow.IsNull()) return nullptr;
	
	bool bLeftOrRight = SnappedParams.bLeftOrRight;
	FVSWallRunSettings* SettingsPtr = SnappedParams.SettingsRow.GetRow<FVSWallRunSettings>(nullptr);
	FVSAnimSequenceReference* Anim  = (!bLeftOrRight ? SettingsPtr->LeftAnims.StartAnim : SettingsPtr->RightAnims.StartAnim).GetRow<FVSAnimSequenceReference>(nullptr);
	return Anim;
}

UAnimSequenceBase* UVSChrMovAnimFeature_WallRunMovement::GetCycleAnimSequence() const
{
	if (!ChrMovFeature_WallRunMovement.IsValid()) return nullptr;
	const FVSWallRunSnappedParams& SnappedParams = ChrMovFeature_WallRunMovement->GetWallRunSnappedParams();
	if (SnappedParams.SettingsRow.IsNull()) return nullptr;
	
	bool bLeftOrRight = SnappedParams.bLeftOrRight;
	FVSWallRunSettings* SettingsPtr = SnappedParams.SettingsRow.GetRow<FVSWallRunSettings>(nullptr);
	return !bLeftOrRight ? SettingsPtr->LeftAnims.CycleAnim : SettingsPtr->RightAnims.CycleAnim;
}

FVSAnimSequenceReference* UVSChrMovAnimFeature_WallRunMovement::GetEndAnimSequenceReferencePtr() const
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || ChrMovFeature_WallRunMovement->GetWallRunState() != EVSWallRunState::Ending) return nullptr;
	const FVSWallRunSnappedParams& SnappedParams = ChrMovFeature_WallRunMovement->GetWallRunSnappedParams();
	if (SnappedParams.SettingsRow.IsNull()) return nullptr;
	
	bool bLeftOrRight = SnappedParams.bLeftOrRight;
	FVSWallRunSettings* SettingsPtr = SnappedParams.SettingsRow.GetRow<FVSWallRunSettings>(nullptr);
	FVSAnimSequenceReference* Anim  = (!bLeftOrRight ? SettingsPtr->LeftAnims.EndAnim : SettingsPtr->RightAnims.EndAnim).GetRow<FVSAnimSequenceReference>(nullptr);
	return Anim;
}

void UVSChrMovAnimFeature_WallRunMovement::SetupWallRunStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || !HasValidWallRunStartAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = GetStartAnimSequenceReferencePtr();

	if (UAnimSequenceBase* NewAnim = StartAnim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, StartAnim->GetSafePlayTimeRange().X);
	AnimData.LastActionID = ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID;
}

void UVSChrMovAnimFeature_WallRunMovement::UpdateWallRunStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || !HasValidWallRunStartAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = GetStartAnimSequenceReferencePtr();
	
	if (AnimData.LastActionID != ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID && ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID != INDEX_NONE && ChrMovFeature_WallRunMovement->IsWallRunMode())
	{
		if (UAnimSequenceBase* NewAnim = StartAnim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, StartAnim->GetSafePlayTimeRange().X);
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
		AnimData.LastActionID = ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID;
	}
	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, StartAnim->PlayRate);
}

void UVSChrMovAnimFeature_WallRunMovement::UpdateWallRunCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || !GetCycleAnimSequence()) return;
	
	FSequencePlayerReference SequencePlayer;
	bool bResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, bResult);
	if (!bResult) return;
	
	if (UAnimSequenceBase* NewAnim = GetCycleAnimSequence())
	{
		if (NewAnim != USequencePlayerLibrary::GetSequencePure(SequencePlayer))
		{
			USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, NewAnim, 0.2f);
		}
	}

	if (USequencePlayerLibrary::GetSequencePure(SequencePlayer))
	{
		const FVector& VelocityWallAdjusted2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetAnimVelocity2D());
		UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(SequencePlayer, VelocityWallAdjusted2DRS.Size());
	}	
}

void UVSChrMovAnimFeature_WallRunMovement::SetupWallRunEndAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || !HasValidWallRunEndAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* EndAnim = GetEndAnimSequenceReferencePtr();

	if (UAnimSequenceBase* NewAnim = EndAnim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, EndAnim->GetSafePlayTimeRange().X);
}

void UVSChrMovAnimFeature_WallRunMovement::UpdateWallRunEndAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!ChrMovFeature_WallRunMovement.IsValid() || !HasValidWallRunEndAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* EndAnim = GetEndAnimSequenceReferencePtr();
	
	// if (AnimData.LastActionID != ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID && ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID != INDEX_NONE && ChrMovFeature_WallRunMovement->IsWallRunMode())
	// {
	// 	if (UAnimSequenceBase* NewAnim = EndAnim->AnimSequence)
	// 	{
	// 		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	// 	}
	// 	
	// 	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, EndAnim->GetSafePlayTimeRange().X);
	// 	UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
	// 	AnimData.LastActionID = ChrMovFeature_WallRunMovement->GetWallRunCachedParams().ActionID;
	// }
	
	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, EndAnim->PlayRate);
}
