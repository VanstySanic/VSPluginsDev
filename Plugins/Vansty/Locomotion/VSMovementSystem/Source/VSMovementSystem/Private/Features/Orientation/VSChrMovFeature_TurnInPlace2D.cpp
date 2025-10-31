// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_TurnInPlace2D.h"
#include "VSCharacterMovementUtils.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovFeature_TurnInPlace2D::UVSChrMovFeature_TurnInPlace2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovFeature_TurnInPlace2D::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (IsMoving2D() || HasMovementInput2D())
	{
		if (IsTurningInPlace()) StopTurnInPlaceInternal();
		MovementData.CachedParams.TriggerDelayedTime = 0.f;
	}

	MovementData.CachedParams.CheckBlockRemainedTime = FMath::Max(MovementData.CachedParams.CheckBlockRemainedTime - DeltaTime, 0.f);
	if (GetOwnerActor()->HasAuthority())
	{
		if (MovementData.CachedParams.CheckBlockRemainedTime <= 0.f)
		{
			TurnInPlaceCheck(DeltaTime);
		}
		else
		{
			MovementData.CachedParams.TriggerDelayedTime = 0.f;
		}
	}
}

void UVSChrMovFeature_TurnInPlace2D::OnFeatureDeactivated_Implementation()
{
	if (IsTurningInPlace()) { StopTurnInPlaceInternal(); }
	
	Super::OnFeatureDeactivated_Implementation();
}

void UVSChrMovFeature_TurnInPlace2D::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UVSChrMovFeature_TurnInPlace2D, ReplicatedSnappedParams, SharedParams);
}

void UVSChrMovFeature_TurnInPlace2D::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	MovementData.CurrentSettingsRow = DefaultSettingsRow;
	UpdateMovementTagQueryStates(EVSMovementEvent::StateChange_MovementMode);
}

bool UVSChrMovFeature_TurnInPlace2D::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsTurningInPlace();
}

void UVSChrMovFeature_TurnInPlace2D::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	FVSAnimSequenceReference* Anim = MovementData.SnappedParams.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	
	/** Update character rotation. */
	const float AnimRotationYawCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, MovementData.CachedParams.AnimPlayedTime);
	const float DeltaAnimRotationYawCurveValue = AnimRotationYawCurveValue - MovementData.CachedParams.LastUpdatedAnimRotationYawCurveValue;
	const float RotateScale = MovementData.SnappedParams.DeltaAngle / MovementData.CachedParams.AnimRotationAngle;
	
	const float DeltaYaw = DeltaAnimRotationYawCurveValue * RotateScale;
	GetCharacter()->AddActorLocalRotation(FRotator(0.0, DeltaYaw, 0.0));
	
	MovementData.CachedParams.LastUpdatedAnimRotationYawCurveValue = AnimRotationYawCurveValue;
	
	/** Update time and state. */
	const FVector2D& AnimSafePlayTimeRange = Anim->GetSafePlayTimeRange();
	MovementData.CachedParams.AnimPlayedTime = FMath::Min(MovementData.CachedParams.AnimPlayedTime + DeltaTime * Anim->PlayRate * MovementData.SnappedParams.PlayRateMultiplier, AnimSafePlayTimeRange.Y);
	if (FMath::IsNearlyEqual(MovementData.CachedParams.AnimPlayedTime, AnimSafePlayTimeRange.Y, 0.01f))
	{
		StopTurnInPlaceInternal();
	}
}

void UVSChrMovFeature_TurnInPlace2D::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	UpdateMovementTagQueryStates(TagEvent);
}

bool UVSChrMovFeature_TurnInPlace2D::IsTurningInPlace() const
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	return GameplayTagController ? (GameplayTagController->GetTagCount(EVSMovementState::TurnInPlace) >= 1) : false;
}

void UVSChrMovFeature_TurnInPlace2D::TurnInPlaceCheck(float DeltaTime)
{
	if (!GetCharacter()) return;
	if (IsMoving2D() || HasMovementInput2D() || !MovementData.bMatchesEntranceTagQuery)
	{
		MovementData.CachedParams.TriggerDelayedTime = 0.f;
		return;
	}

	/** Check if we are after the time to start a new turn in place check when turning. */
	if (IsTurningInPlace())
	{
		const FVSAnimSequenceReference* Anim = MovementData.SnappedParams.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
		if (!Anim || !Anim->IsValid() || !Anim->HasTimeMark(AnimRecoveryTimeMarkName)) return;
		if (MovementData.CachedParams.AnimPlayedTime < Anim->GetMarkTime(AnimRecoveryTimeMarkName)) return;
	}
	
	const FVSTurnInPlaceSettings2D* Settings = MovementData.CurrentSettingsRow.GetRow<FVSTurnInPlaceSettings2D>(nullptr);
	if (!Settings || !Settings->IsValid())
	{
		MovementData.CachedParams.TriggerDelayedTime = 0.f;
		return;
	}

	/** Evaluate target rotation. */
	FRotator EvaluatedRotation = GetMovementFeatureAgent()->EvaluateOrientation(FVSMovementOrientationEvaluateParams(OrientationEvaluateType));

	/** Calculate the delta yaw between the evaluated and the current. */
	const FQuat& WorldToUpRotation = FQuat::FindBetweenNormals(FVector::UpVector, GetUpDirection());
	const FQuat& UpToWorldRotation = WorldToUpRotation.Inverse();
	const FRotator& EvaluatedRotationDS = UKismetMathLibrary::ComposeRotators(EvaluatedRotation, UpToWorldRotation.Rotator());
	const FRotator& CurrentRotationDS = UKismetMathLibrary::ComposeRotators(GetCharacter()->GetActorRotation(), UpToWorldRotation.Rotator());
	const float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(EvaluatedRotationDS, CurrentRotationDS).Yaw;
	/** Check the angle range and trigger delay time. */
	const float TriggerDelayTime = UKismetMathLibrary::MapRangeClamped(FMath::Abs(DeltaYaw), Settings->AngleThreshold, 180.f, Settings->TriggerDelayTimeRange.X, Settings->TriggerDelayTimeRange.Y);
	if (FMath::Abs(DeltaYaw) > Settings->AngleThreshold)
	{
		MovementData.CachedParams.TriggerDelayedTime = FMath::Min(MovementData.CachedParams.TriggerDelayedTime + DeltaTime, Settings->TriggerDelayTimeRange.Y);
	}
	else
	{
		// MovementData.CachedParams.TriggerDelayedTime = bDeclineTriggerDelayTime ? FMath::Max(MovementData.CachedParams.TriggerDelayedTime - DeltaTime, 0.f) : 0.f;
		MovementData.CachedParams.TriggerDelayedTime = 0.f;
	}
	if (MovementData.CachedParams.TriggerDelayedTime < TriggerDelayTime) return;

	/** Check the animation. */
	FVSDataTableRowHandleWrap SettingsRow = FVSDataTableRowHandleWrap();
	FVSDataTableRowHandleWrap AnimRow = FVSDataTableRowHandleWrap();
	FVSAnimSequenceReference* Anim = nullptr;
	for (const auto& AngledAnim : Settings->AngledAnims)
	{
		if (!UKismetMathLibrary::InRange_FloatFloat(DeltaYaw, AngledAnim.Key.X, AngledAnim.Key.Y)) continue;
		Anim = AngledAnim.Value.GetRow<FVSAnimSequenceReference>(nullptr);
		if (!Anim || !Anim->IsValid() || !UVSAnimationLibrary::AnimationHasCurve(Anim->AnimSequence, AnimRotationYawCurveName)) continue;
	
		/** Found valid angled animation. */
		AnimRow = AngledAnim.Value;
		break;
	}
	if (AnimRow.IsNull() || !Anim) return;

	/** Start new turning in place process. */
	const FVector2D& AnimSafePlayTimeRange = Anim->GetSafePlayTimeRange();
	
	MovementData.CachedParams.TriggerDelayedTime = 0.f;
	MovementData.CachedParams.AnimRotationAngle = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.Y) - UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.X);
	MovementData.CachedParams.LastUpdatedAnimRotationYawCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.X);
	MovementData.CachedParams.AnimPlayedTime = AnimSafePlayTimeRange.X;

	MovementData.SnappedParams.DeltaAngle = Settings->bRotationNoScale ? UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.Y) : DeltaYaw;
	MovementData.SnappedParams.SettingsRow = MovementData.CurrentSettingsRow;
	MovementData.SnappedParams.AnimRow = AnimRow;
	MovementData.SnappedParams.ActionID = FMath::RandRange(0, INT16_MAX);
	//MovementData.AnimParams.PlayRateMultiplier = UKismetMathLibrary::MapRangeClamped(DeltaYaw, Settings->PlayRateMultiplierByAngle.X, Settings->PlayRateMultiplierByAngle.Y, Settings->PlayRateMultiplierByAngle.Z, Settings->PlayRateMultiplierByAngle.W);

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	if (!IsTurningInPlace())
	{
		GameplayTagController->SetTagCount(EVSMovementState::TurnInPlace, 1);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::Action_TurnInPlace_Start);
	}
	else
	{
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::Action_TurnInPlace_Connect);
	}

	ReplicatedSnappedParams = MovementData.SnappedParams;
	MARK_PROPERTY_DIRTY_FROM_NAME(UVSChrMovFeature_TurnInPlace2D, ReplicatedSnappedParams, this);
}

void UVSChrMovFeature_TurnInPlace2D::StopTurnInPlaceInternal()
{
	if (!IsTurningInPlace()) return;
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(EVSMovementState::TurnInPlace, 0);
	GameplayTagController->NotifyTagsUpdated();
	GameplayTagController->NotifyTagEvent(EVSMovementEvent::Action_TurnInPlace_End);

	MovementData.SnappedParams = FVSTurnInPlaceSnappedParams2D();
	MovementData.CachedParams = FMovementData::FMovementCachedParams();
	ReplicatedSnappedParams = FVSTurnInPlaceSnappedParams2D();
}

void UVSChrMovFeature_TurnInPlace2D::UpdateMovementTagQueryStates(const FGameplayTag& TagEvent)
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	if (!GameplayTagController) return;
	FGameplayTagContainer GameplayTags;
	GameplayTagController->GetOwnedGameplayTags(GameplayTags);

	if (GetOwnerActor()->HasAuthority())
	{
		if (RefreshSettingsRowTagQuery.Matches(TagEvent, GameplayTags))
		{
			FVSDataTableRowHandleWrap NewRow = DefaultSettingsRow;
			for (const auto& TaggedSettingRow : TagQueriedSettingRows)
			{
				if (TaggedSettingRow.Value.Matches(TagEvent, GameplayTags))
				{
					FVSTurnInPlaceSettings2D* Settings = TaggedSettingRow.Key.GetRow<FVSTurnInPlaceSettings2D>(nullptr);
					if (!Settings || !Settings->IsValid()) continue;
					NewRow = TaggedSettingRow.Key;
					break;
				}
			}
			if (MovementData.CurrentSettingsRow != NewRow)
			{
				MovementData.CurrentSettingsRow = NewRow;
				MovementData.CachedParams.TriggerDelayedTime = 0.f;
				if (IsTurningInPlace()) { StopTurnInPlaceInternal(); }
			}
		}
		
		/** Check time and state. */
		MovementData.bMatchesEntranceTagQuery = EntranceTagQuery.IsEmpty() || EntranceTagQuery.Matches(GameplayTags);
		if (!MovementData.bMatchesEntranceTagQuery)
		{
			MovementData.CachedParams.TriggerDelayedTime = 0.f;
		}
		
		for (const auto& QueriedCheckBlockTime : TagQueriedCheckBlockTimes)
		{
			if (QueriedCheckBlockTime.Value.Matches(TagEvent, GameplayTags))
			{
				MovementData.CachedParams.TriggerDelayedTime = 0.f;
				MovementData.CachedParams.CheckBlockRemainedTime = QueriedCheckBlockTime.Key;
				break;
			}
		}
	}

	if (IsTurningInPlace() && AutoBreakTagQuery.Matches(TagEvent, GameplayTags))
	{
		MovementData.CachedParams.TriggerDelayedTime = 0.f;
		StopTurnInPlaceInternal();
	}
}

void UVSChrMovFeature_TurnInPlace2D::OnRep_ReplicatedSnappedParams()
{
	FVSAnimSequenceReference* Anim = ReplicatedSnappedParams.AnimRow.GetRow<FVSAnimSequenceReference>();
	if (!Anim || !Anim->IsValid()) return;

	const FVector2D& AnimSafePlayTimeRange = Anim->GetSafePlayTimeRange();
	
	MovementData.CachedParams.TriggerDelayedTime = 0.f;
	MovementData.CachedParams.AnimRotationAngle = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.Y) - UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.X);
	MovementData.CachedParams.LastUpdatedAnimRotationYawCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, AnimRotationYawCurveName, AnimSafePlayTimeRange.X);
	MovementData.CachedParams.AnimPlayedTime = AnimSafePlayTimeRange.X;
	MovementData.CurrentSettingsRow = ReplicatedSnappedParams.SettingsRow;

	MovementData.SnappedParams = ReplicatedSnappedParams;
	MovementData.SnappedParams.ActionID = FMath::RandRange(0, INT16_MAX);

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	if (!IsTurningInPlace())
	{
		GameplayTagController->SetTagCount(EVSMovementState::TurnInPlace, 1);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::Action_TurnInPlace_Start);
	}
	else
	{
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::Action_TurnInPlace_Connect);
	}
}
