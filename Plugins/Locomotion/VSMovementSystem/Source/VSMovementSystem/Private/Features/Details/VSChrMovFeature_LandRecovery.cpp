// Copyright VanstySanic. All Rights Reserved.

#include "Features/Details/VSChrMovFeature_LandRecovery.h"
#include "VSMovementSystemSettings.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovFeature_LandRecovery::UVSChrMovFeature_LandRecovery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSChrMovFeature_LandRecovery::IsLandRecovering() const
{
	UVSGameplayTagController* GameplayTagSet = GetGameplayTagController();
	return GameplayTagSet ? (GameplayTagSet->GetTagCount(EVSMovementState::LandRecovery) >= 1) : false;
}

bool UVSChrMovFeature_LandRecovery::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsLandRecovering() && MovementData.Anim;
}

void UVSChrMovFeature_LandRecovery::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	MovementData.AnimPlayedTime = FMath::Min(MovementData.AnimPlayedTime + DeltaTime * MovementData.Anim->PlayRate * MovementData.AnimParams.PlayRateMultiplier, MovementData.Anim->GetSafePlayTimeRange().Y);
	if (FMath::IsNearlyEqual(MovementData.AnimPlayedTime, MovementData.Anim->GetSafePlayTimeRange().Y, 0.01f)
		|| ((!bEarlyOutWithInputWhenAdditive || MovementData.AnimParams.AnimPlayPolicy != EVSLandRecoveryAnimPlayPolicy::PlayAsAdditive)
			&& MovementData.Anim->HasTimeMark(AnimEarliestOutWithInputTimeMarkName) && MovementData.AnimPlayedTime >= MovementData.Anim->GetMarkTime(AnimEarliestOutWithInputTimeMarkName) && HasMovementInput2D()))
	{
		StopLandRecoveryInternal();
	}

	/** Sync movement with animation. */
	if (MovementData.Settings->AnimPlayPolicy == EVSLandRecoveryAnimPlayPolicy::PlayWithMovement)
	{
		FVector AnimMovementCurves = FVector::ZeroVector;
		AnimMovementCurves.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), MovementData.AnimPlayedTime);
		AnimMovementCurves.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.AnimPlayedTime);
		AnimMovementCurves.Z = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.AnimPlayedTime);

		const FVector& DeltaCurvesRS = AnimMovementCurves - MovementData.LastUpdatedAnimMovementCurves;
		const FVector& DeltaCurvesWS = GetCharacter()->GetActorTransform().TransformVector(DeltaCurvesRS);

		const FVector& MovementDirection = IsMoving2D() ? GetVelocity2D().GetSafeNormal() : DeltaCurvesWS.GetSafeNormal();
		const FQuat& CurveToMovementRotationWS = FQuat::FindBetweenVectors(DeltaCurvesWS, MovementDirection);
		const FVector& DeltaMovementWS = UKismetMathLibrary::Quat_RotateVector(CurveToMovementRotationWS, DeltaCurvesWS);

		GetCharacterMovement()->Velocity = DeltaMovementWS / DeltaTime;
		GetCharacter()->AddActorWorldOffset(DeltaMovementWS, true);
		MovementData.LastUpdatedAnimMovementCurves = AnimMovementCurves;
	}
}

void UVSChrMovFeature_LandRecovery::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		const bool bIsInWalkingMode = GetMovementMode() == EVSMovementMode::Walking || GetMovementMode() == EVSMovementMode::NavWalking;
		const bool bPrevInWalkingMode = GetPrevMovementMode() == EVSMovementMode::Walking || GetPrevMovementMode() == EVSMovementMode::NavWalking;
		if (bIsInWalkingMode && !bPrevInWalkingMode/* && GetOwnerActor()->HasAuthority()*/)
		{
			CheckLandRecovery();
		}
		else if (bPrevInWalkingMode && !bIsInWalkingMode)
		{
			StopLandRecoveryInternal();
		}
	}
}

void UVSChrMovFeature_LandRecovery::CheckLandRecovery()
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	UVSCharacterMovementFeatureAgent* FeatureAgent = GetMovementFeatureAgent();
	FGameplayTagContainer GameplayTags;
	GameplayTagController->GetOwnedGameplayTags(GameplayTags);
	
	const FTransform& CharacterTransformWS = GetCharacter()->GetActorTransform();
	const FVector& VelocityZRS = CharacterTransformWS.InverseTransformVector(GetCharacterMovement()->GetLastUpdateVelocity().ProjectOnTo(GetUpDirection()));
	
	MovementData.Settings = nullptr;
	MovementData.Anim = nullptr;
	MovementData.AnimPlayedTime = 0.f;
	MovementData.LastUpdatedAnimMovementCurves = FVector::ZeroVector;
	
	FVSDataTableRowHandleWrap SettingRow = DefaultSettingRows;
	for (const auto& TaggedSettingRow : QueriedSettingRows)
	{
		if (!TaggedSettingRow.Value.Matches(GameplayTags)) continue;
		FVSLandRecoverySettings* LandRecoverySettings = TaggedSettingRow.Key.GetRow<FVSLandRecoverySettings>(nullptr);
		if (!LandRecoverySettings || !LandRecoverySettings->IsValid()) continue;
		if (!LandRecoverySettings->Limits.Matches(FeatureAgent)) continue;
		SettingRow = TaggedSettingRow.Key;
		break;
	}

	FVSLandRecoverySettings* Settings = SettingRow.GetRow<FVSLandRecoverySettings>(nullptr);
	if (!Settings || !Settings->IsValid()) return;
	
	const FDataTableRowHandle& AnimRow = Settings->RecoveryAnimRow;
	FVSAnimSequenceReference* Anim = AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (!Anim || !Anim->IsValid()) return;

	MovementData.Settings = Settings;
	MovementData.Anim = Anim;
	MovementData.AnimPlayedTime = MovementData.Anim->HasTimeMark(AnimReachGroundTimeMarkName) ? MovementData.Anim->GetMarkTime(AnimReachGroundTimeMarkName) : MovementData.Anim->GetSafePlayTimeRange().X;
	MovementData.LastUpdatedAnimMovementCurves.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), MovementData.Anim->GetSafePlayTimeRange().X);
	MovementData.LastUpdatedAnimMovementCurves.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.Anim->GetSafePlayTimeRange().X);
	MovementData.LastUpdatedAnimMovementCurves.Z = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.Anim->GetSafePlayTimeRange().X);
	
	MovementData.AnimParams.SettingsRow = SettingRow;
	MovementData.AnimParams.AnimRow = AnimRow;
	MovementData.AnimParams.AnimPlayPolicy = MovementData.Settings->AnimPlayPolicy;
	MovementData.AnimParams.ActionID = FMath::RandRange(0, INT16_MAX);
	MovementData.AnimParams.AdditiveAlpha = UKismetMathLibrary::MapRangeClamped(VelocityZRS.Size(), MovementData.Settings->AdditiveAlphaClampBySpeedZ.X, MovementData.Settings->AdditiveAlphaClampBySpeedZ.Y, MovementData.Settings->AdditiveAlphaClampBySpeedZ.Z, MovementData.Settings->AdditiveAlphaClampBySpeedZ.W);
	// MovementData.AnimParams.PlayRateMultiplier = 1.f;
	if (!MovementData.Settings->AdditiveAlphaMultipliers.IsEmpty())
	{
		for (const auto& AdditiveAlphaMultiplier : MovementData.Settings->AdditiveAlphaMultipliers)
		{
			if (AdditiveAlphaMultiplier.Value.Matches(GameplayTags))
			{
				MovementData.AnimParams.AdditiveAlpha *= AdditiveAlphaMultiplier.Key;
			}
		}
	}

	GameplayTagController->SetTagCount(EVSMovementState::LandRecovery, 1);
	GameplayTagController->NotifyTagsUpdated();
}

void UVSChrMovFeature_LandRecovery::StopLandRecoveryInternal()
{
	if (!IsLandRecovering()) return;
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(EVSMovementState::LandRecovery, 0);
	GameplayTagController->NotifyTagsUpdated();
}

void UVSChrMovFeature_LandRecovery::OnRep_ReplicatedSettingsRow()
{
	
}
