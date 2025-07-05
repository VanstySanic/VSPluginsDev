// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovAnimFeature_WalkingMovement.h"

#include "AnimCharacterMovementLibrary.h"
#include "AnimDistanceMatchingLibrary.h"
#include "AnimExecutionContextLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "VSCharacterMovementUtils.h"
#include "VSMovementAnimUtils.h"
#include "VSMovementSystemSettings.h"
#include "Features/VSCharacterMovementAnimFeatureAgent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Movement/VSChrMovFeature_WalkingMovement.h"
#include "Features/Orientation/VSChrMovFeature_OrientationControl2D.h"
#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Types/VSChrMovOrientationTypes.h"
#include "Types/VSMovementAnimTags.h"
#include "Types/VSMovementAnimTypes.h"

UVSChrMovAnimFeature_WalkingMovement::UVSChrMovAnimFeature_WalkingMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovAnimFeature_WalkingMovement::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	ChrMovFeature_WalkingMovement = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_WalkingMovement>();
	check(ChrMovFeature_WalkingMovement.IsValid());
	ChrMovFeature_OrientationControl2D = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_OrientationControl2D>();
	check(ChrMovFeature_OrientationControl2D.IsValid());

	AnimData.AnimSettingsPtr = AnimSettingsRow.GetRow<FVSWalkingMovementAnimSettings>(nullptr);
	check(AnimData.AnimSettingsPtr);
}

void UVSChrMovAnimFeature_WalkingMovement::UpdateAnimationThreadSafe_Implementation(float DeltaTime)
{
	Super::UpdateAnimationThreadSafe_Implementation(DeltaTime);

	const FGameplayTag SnappedStance = GetStance();
	
	bStanceChangedThisFrame = AnimData.CachedStance != SnappedStance;
	
	bGaitChangedThisFrame = !bStanceChangedThisFrame && AnimData.CachedGait != GetGait();
	
	EvaluatedTargetOrientationForInput = EvaluateTargetOrientationForInput();
	
	AnimData.CachedStance = SnappedStance;
	AnimData.CachedGait = GetGait();
}

FGameplayTag UVSChrMovAnimFeature_WalkingMovement::GetStance() const
{
	return ChrMovFeature_WalkingMovement.IsValid() ? ChrMovFeature_WalkingMovement->GetStance() : FGameplayTag::EmptyTag;
}

FGameplayTag UVSChrMovAnimFeature_WalkingMovement::GetGait() const
{
	return ChrMovFeature_WalkingMovement.IsValid() ? ChrMovFeature_WalkingMovement->GetGait() : FGameplayTag::EmptyTag;
}

TMap<FGameplayTag, float> UVSChrMovAnimFeature_WalkingMovement::GetLeanAmounts() const
{
	if (!GetCharacter()) return TMap<FGameplayTag, float>();
	if (GetRealAcceleration2D().IsNearlyZero(1.f)) return TMap<FGameplayTag, float>();
	TMap<FGameplayTag, float> LeanAmounts;

	const FVector& RealAcceleration2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetRealAcceleration2D());
	
	const float Sum = FMath::Abs(RealAcceleration2DRS.X) + FMath::Abs(RealAcceleration2DRS.Y) + FMath::Abs(RealAcceleration2DRS.Z);
	const FVector& DirectionNormalized = RealAcceleration2DRS / Sum;
	LeanAmounts.Emplace(EVSAnimDirection::FX, FMath::Clamp(DirectionNormalized.X, 0.f, 1.f));
	LeanAmounts.Emplace(EVSAnimDirection::BX, -FMath::Clamp(DirectionNormalized.X, -1.f, 0.f));
	LeanAmounts.Emplace(EVSAnimDirection::XL, -FMath::Clamp(DirectionNormalized.Y, -1.f, 0.f));
	LeanAmounts.Emplace(EVSAnimDirection::XR, FMath::Clamp(DirectionNormalized.Y, 0.f, 1.f));

	return LeanAmounts;
}

float UVSChrMovAnimFeature_WalkingMovement::GetLeanAlpha() const
{
	const FVector& VelocityWallAdjusted2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetAnimVelocity2D());
	const FVector& RealAcceleration2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetRealAcceleration2D());
	const float SpeedAlpha = FMath::Clamp(VelocityWallAdjusted2DRS.Size() / AnimData.AnimSettingsPtr->LeanMaxSpeed, 0.f, 1.f);
	const float AccelerationAlpha = FMath::Clamp(RealAcceleration2DRS.Size() / AnimData.AnimSettingsPtr->LeanMaxAcceleration, 0.f, 1.f);
	return FMath::Clamp(SpeedAlpha * AccelerationAlpha, 0.f, 1.f);
}

FRotator UVSChrMovAnimFeature_WalkingMovement::EvaluateTargetOrientationForInput() const
{
	if (!GetCharacter() || !ChrMovFeature_OrientationControl2D.IsValid()) return FRotator::ZeroRotator;
	FVSOrientationEvaluateParams EvaluateParams;
	EvaluateParams.Type = ChrMovFeature_OrientationControl2D->GetMovingOrientationEvaluateType();
	EvaluateParams.NamedParams.VectorParams.Emplace(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.Velocity, GetMovementInput2D());
	EvaluateParams.NamedParams.VectorParams.Emplace(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.MovementInput, GetMovementInput2D());
	FRotator Rotation = GetCharacter()->GetActorRotation();
	UVSCharacterMovementUtils::EvaluateCharacterMovementOrientation(GetCharacter(), Rotation, EvaluateParams);
	return Rotation;
}

float UVSChrMovAnimFeature_WalkingMovement::GetPossibleInertialBlendingTime() const
{
	if (!AnimData.AnimSettingsPtr) return 0.2f;
	return (bStanceChangedThisFrame || bGaitChangedThisFrame) ? AnimData.AnimSettingsPtr->StanceGaitChangeInertialBlendingTime : AnimData.AnimSettingsPtr->DirectionChangeInertialBlendingTime;
}

FVSWalkingMovementAnimSettings UVSChrMovAnimFeature_WalkingMovement::GetWalkingMovementSettings() const
{
	return AnimData.AnimSettingsPtr ? (*AnimData.AnimSettingsPtr) : FVSWalkingMovementAnimSettings();
}

bool UVSChrMovAnimFeature_WalkingMovement::CanBreakFromStartToCycle() const
{
	if (bStanceChangedThisFrame || bGaitChangedThisFrame) return true;

	if (!FMath::IsNearlyEqual(StartStrideWarpingAlpha, 1.f)) return false;
	
	const FVector& VelocityRelativeToEvaluatedOrientation = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetAnimVelocity2D().GetSafeNormal());
	const FVector& InputRelativeToEvaluatedOrientation = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D().GetSafeNormal());
	const FVector& StartInputRelativeToEvaluatedOrientation = StartInputRelativeToEvaluated.GetSafeNormal();

	if (VelocityRelativeToEvaluatedOrientation.Dot(StartInputRelativeToEvaluatedOrientation) < UKismetMathLibrary::DegCos(22.5)) return true;
	if (InputRelativeToEvaluatedOrientation.Dot(StartInputRelativeToEvaluatedOrientation) < UKismetMathLibrary::DegCos(22.5)) return true;
	
	return false;
}

bool UVSChrMovAnimFeature_WalkingMovement::CanEnterPivotState() const
{
	if (!IsMoving2D() || !HasMovementInput2D()) return false;
	/** The input direction is differ from current velocity. */
	return GetVelocity2D().GetSafeNormal().Dot(GetMovementInput2D().GetSafeNormal()) < -UKismetMathLibrary::DegCos(22.5);
}

bool UVSChrMovAnimFeature_WalkingMovement::CanRePivot() const
{
	if (!IsMoving2D() || !HasMovementInput2D()) return false;
	
	const FVector& VelocityRelativeToEvaluatedOrientation = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetAnimVelocity2D().GetSafeNormal());
	const FVector& InputRelativeToEvaluatedOrientation = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D().GetSafeNormal());
	const FVector& PivotInputDirectionRelativeToEvaluatedOrientation = PivotInputRelativeToEvaluated.GetSafeNormal();

	/** Velocity and input direction angle check. */
	if (FMath::Abs(InputRelativeToEvaluatedOrientation.Dot(VelocityRelativeToEvaluatedOrientation)) < UKismetMathLibrary::DegCos(22.5)) return false;

	/** Current input needs to differ from previous. */
	if (InputRelativeToEvaluatedOrientation.GetSafeNormal().Dot(PivotInputDirectionRelativeToEvaluatedOrientation) > -UKismetMathLibrary::DegCos(22.5)) return false;
	
	return true;
}

bool UVSChrMovAnimFeature_WalkingMovement::CanBreakFromPivotCycle() const
{
	if (bStanceChangedThisFrame || bGaitChangedThisFrame) return true;

	if (CanRePivot()) return !IsValid(CalcPivotAnim());
	
	if (!FMath::IsNearlyEqual(PivotStrideWarpingAlpha, 1.f)) return false;
	
	const FVector& VelocityRelativeToEvaluatedOrientation = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetAnimVelocity2D().GetSafeNormal());
	const FVector& InputRelativeToEvaluatedOrientation = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D().GetSafeNormal());
	const FVector& PivotInputRelativeToEvaluatedOrientation = PivotInputRelativeToEvaluated.GetSafeNormal();
	
	if (VelocityRelativeToEvaluatedOrientation.Dot(PivotInputRelativeToEvaluatedOrientation) < UKismetMathLibrary::DegCos(22.5)) return true;
	if (InputRelativeToEvaluatedOrientation.Dot(PivotInputRelativeToEvaluatedOrientation.GetSafeNormal()) < UKismetMathLibrary::DegCos(22.5)) return true;
	
	return false;
}

UAnimSequence* UVSChrMovAnimFeature_WalkingMovement::CalcIdleAnim() const
{
	if (!GetCharacter() || !AnimData.AnimSettingsPtr || IsMoving2D()) return nullptr;
	return AnimData.AnimSettingsPtr->IdleAnims.FindRef(GetStance());
}

UAnimSequence* UVSChrMovAnimFeature_WalkingMovement::CalcStanceTransitionAnim() const
{
	if (!AnimData.AnimSettingsPtr || IsMoving2D() || !ChrMovFeature_WalkingMovement.IsValid()) return nullptr;

	FVSGameplayTagKey Keys = TArray
	{
		ChrMovFeature_WalkingMovement->GetPrevStance(),
		GetStance(),
	};
	
	return AnimData.AnimSettingsPtr->StanceTransitionAnims.FindRef(Keys);
}

UAnimSequence* UVSChrMovAnimFeature_WalkingMovement::CalcStartAnim() const
{
	if (!GetCharacter() || !ChrMovFeature_OrientationControl2D.IsValid() || !AnimData.AnimSettingsPtr || !HasMovementInput2D()) return nullptr;
	
	const FQuat& UpVectorToWorldRotation = FQuat::FindBetweenNormals(GetUpDirection(), FVector::UpVector);
	const FRotator& EvaluatedOrientationWithMovementDS = UKismetMathLibrary::ComposeRotators(EvaluatedTargetOrientationForInput, UpVectorToWorldRotation.Rotator());
	const FRotator& InputOrientationDS = UKismetMathLibrary::Quat_RotateVector(UpVectorToWorldRotation, GetMovementInput2D()).Rotation();
	const float DeltaYaw = UKismetMathLibrary::NormalizeAxis(InputOrientationDS.Yaw - EvaluatedOrientationWithMovementDS.Yaw);
	const FGameplayTag& Direction = UVSMovementAnimUtils::CalcAnimDirectionByAngle2D(DeltaYaw, GetAnimFeatureAgent()->DirectionBufferAngle, GetAnimVelocityDirectionToCharacter2D());
	
	FVSGameplayTagKey Keys = TArray
	{
		GetStance(),
		GetGait(),
		Direction
	};

	return AnimData.AnimSettingsPtr->StartAnims.GetAnimSequence(Keys);
}

UAnimSequence* UVSChrMovAnimFeature_WalkingMovement::CalcCycleAnim() const
{
	if (!GetCharacter() || !ChrMovFeature_OrientationControl2D.IsValid() || !AnimData.AnimSettingsPtr || (!IsMoving2D() && !HasMovementInput2D())) return nullptr;

	// const FQuat& UpVectorToWorldRotation = FQuat::FindBetweenNormals(GetUpDirection(), FVector::UpVector);
	// const FRotator& CurrentOrientationDS = UKismetMathLibrary::ComposeRotators(GetCharacter()->GetActorRotation(), UpVectorToWorldRotation.Rotator());
	// const FRotator& MovementOrientationDS = UKismetMathLibrary::Quat_RotateVector(UpVectorToWorldRotation, IsMoving2D() ? GetAnimVelocity2D() : GetMovementInput2D()).Rotation();
	// FRotator EvaluatedRotationWS = GetCharacter()->GetActorRotation();
	// FVSOrientationEvaluateParams EvaluateParams;
	// EvaluateParams.Type = ChrMovFeature_OrientationControl2D->GetMovingOrientationEvaluateType();
	// for (UVSChrMovFeature_OrientationEvaluator* Evaluator : Execute_GetMovementAgentFeature(this)->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
	// {
	// 	if (Evaluator->EvaluateOrientation(EvaluatedRotationWS, EvaluateParams))
	// 	{
	// 		break;
	// 	}
	// }
	// const FRotator& EvaluatedOrientationWithMovementDS = UKismetMathLibrary::ComposeRotators(EvaluatedRotationWS, UpVectorToWorldRotation.Rotator());
	// const float DeltaYaw = UKismetMathLibrary::NormalizeAxis(MovementOrientationDS.Yaw - CurrentOrientationDS.Yaw);
	// const FGameplayTag& Direction = EvaluatedOrientationWithMovementDS.Equals(MovementOrientationDS, AnimData.AnimSettingsPtr->DirectionBufferAngle) ? EVSAnimDirection::FX : UVSLocomotionAnimUtils::CalcAnimDirectionByAngle2D(DeltaYaw, AnimData.AnimSettingsPtr->DirectionBufferAngle, GetVelocityDirectionToCharacter());
	
	FVSGameplayTagKey Keys = TArray
	{
		GetStance(),
		GetGait(),
		// Direction
		GetAnimVelocityDirectionToCharacter2D()
	};
	
	return AnimData.AnimSettingsPtr->CycleAnims.GetAnimSequence(Keys);
}

UAnimSequence* UVSChrMovAnimFeature_WalkingMovement::CalcPivotAnim() const
{
	if (!GetCharacter() || !ChrMovFeature_OrientationControl2D.IsValid() || !AnimData.AnimSettingsPtr || !IsMoving2D() || !HasMovementInput2D()) return nullptr;
	
	const FQuat& UpVectorToWorldRotation = FQuat::FindBetweenNormals(GetUpDirection(), FVector::UpVector);
	const FRotator& EvaluatedOrientationWithMovementDS = UKismetMathLibrary::ComposeRotators(EvaluatedTargetOrientationForInput, UpVectorToWorldRotation.Rotator());
	const FRotator& InputOrientationDS = UKismetMathLibrary::Quat_RotateVector(UpVectorToWorldRotation, GetMovementInput2D()).Rotation();
	const float DeltaYaw = UKismetMathLibrary::NormalizeAxis(InputOrientationDS.Yaw - EvaluatedOrientationWithMovementDS.Yaw);
	const FGameplayTag& Direction = UVSMovementAnimUtils::CalcAnimDirectionByAngle2D(DeltaYaw, GetAnimFeatureAgent()->DirectionBufferAngle, GetAnimVelocityDirectionToCharacter2D());
	
	FVSGameplayTagKey Keys = TArray
	{
		GetStance(),
		GetGait(),
		GetAnimVelocityDirectionToCharacter2D(),
		Direction
	};
	
	return AnimData.AnimSettingsPtr->PivotAnims.GetAnimSequence(Keys);
}

UAnimSequence* UVSChrMovAnimFeature_WalkingMovement::CalcStopAnim() const
{
	if (!GetCharacter() || !AnimData.AnimSettingsPtr || HasMovementInput2D()) return nullptr;

	FVSGameplayTagKey Keys = TArray
	{
		GetStance(),
		GetGait(),
		GetAnimVelocityDirectionToCharacter2D(),
	};
	
	return AnimData.AnimSettingsPtr->StopAnims.GetAnimSequence(Keys);
}

void UVSChrMovAnimFeature_WalkingMovement::UpdateIdleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.AnimSettingsPtr) return;
	FSequencePlayerReference SequencePlayer;
	bool bResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, bResult);
	if (!bResult) return;
	
	if (UAnimSequence* NewAnim = CalcIdleAnim())
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, NewAnim, GetPossibleInertialBlendingTime());
	}
}

void UVSChrMovAnimFeature_WalkingMovement::SetupStanceTransitionAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;
	
	if (UAnimSequence* NewAnim = CalcStanceTransitionAnim())
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
}

void UVSChrMovAnimFeature_WalkingMovement::UpdateStanceTransitionAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	if (bStanceChangedThisFrame)
	{
		if (UAnimSequence* NewAnim = CalcStanceTransitionAnim())
		{
			if (NewAnim != USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator))
			{
				USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, NewAnim, GetPossibleInertialBlendingTime());
			}
		}
		
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
	}

	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
}

void UVSChrMovAnimFeature_WalkingMovement::SetupStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	if (UAnimSequence* NewAnim = CalcStartAnim())
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
	StartInputRelativeToEvaluated = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D());
	StartStrideWarpingAlpha = 0.f;
}

void UVSChrMovAnimFeature_WalkingMovement::UpdateStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.AnimSettingsPtr) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	const float ExplicitTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);
	StartStrideWarpingAlpha = FMath::GetMappedRangeValueClamped(AnimData.AnimSettingsPtr->StartStrideBlendInTimeRange, FVector2D(0.0, 1.0), ExplicitTime);

	/** Allow changing the start animation at the very beginning. */
	if (!FMath::IsNearlyEqual(StartStrideWarpingAlpha, 1.f))
	{
		if (UAnimSequence* NewAnim = CalcStartAnim())
		{
			/** Diffirent anim means direction change. */
			if (NewAnim != USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator))
			{
				USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, NewAnim, GetPossibleInertialBlendingTime());
				StartInputRelativeToEvaluated = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D());
			}
		}
	}
	if (!USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator)) return;

	const FVector& VelocityWallAdjusted2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetAnimVelocity2D());
	const float DeltaDistanceRS = VelocityWallAdjusted2DRS.Size() * UAnimExecutionContextLibrary::GetDeltaTime(Context);
	const float PlayRateClampX = FMath::Lerp(0.2f, AnimData.AnimSettingsPtr->StartPlayRateRange.X, StartStrideWarpingAlpha);
	UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator, DeltaDistanceRS, DistanceCurveName, FVector2D(PlayRateClampX, AnimData.AnimSettingsPtr->StartPlayRateRange.Y));
}

void UVSChrMovAnimFeature_WalkingMovement::UpdateCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.AnimSettingsPtr) return;
	
	FSequencePlayerReference SequencePlayer;
	bool bResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, bResult);
	if (!bResult) return;
	
	if (UAnimSequence* NewAnim = CalcCycleAnim())
	{
		if (NewAnim != USequencePlayerLibrary::GetSequencePure(SequencePlayer))
		{
			USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, NewAnim, GetPossibleInertialBlendingTime());
		}
	}

	if (USequencePlayerLibrary::GetSequencePure(SequencePlayer))
	{
		const FVector& VelocityWallAdjusted2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetAnimVelocity2D());
		UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(SequencePlayer, VelocityWallAdjusted2DRS.Size(), AnimData.AnimSettingsPtr->CyclePlayRateRange);
	}	
}

void UVSChrMovAnimFeature_WalkingMovement::SetupStopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	if (UAnimSequence* NewAnim = CalcStopAnim())
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
}

void UVSChrMovAnimFeature_WalkingMovement::UpdateStopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.AnimSettingsPtr) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;
	if (!USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator)) return;

	const FVector MovementToStopWS = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
			GetVelocity2D(),
			GetCharacterMovement()->bUseSeparateBrakingFriction,
			GetCharacterMovement()->BrakingFriction,
			GetCharacterMovement()->GroundFriction,
			GetCharacterMovement()->BrakingFrictionFactor,
			GetCharacterMovement()->BrakingDecelerationWalking);

	const FVector& MovementToStopWS2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(MovementToStopWS);
	const float DistanceToStop2DRS = MovementToStopWS2DRS.Size();
	
	if (!FMath::IsNearlyZero(DistanceToStop2DRS))
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, DistanceToStop2DRS, DistanceCurveName);
	}
	else
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
	}
}

void UVSChrMovAnimFeature_WalkingMovement::SetupPivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.AnimSettingsPtr) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	if (UAnimSequence* NewAnim = CalcPivotAnim())
	{
		USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, NewAnim, GetPossibleInertialBlendingTime());
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
	}

	PivotInputRelativeToEvaluated = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D());
	// PivotVelocityRelativeToEvaluated = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetAnimVelocity2D());
	PivotStrideWarpingAlpha = 0.f;
	PivotStopTime = 0.f;
}

void UVSChrMovAnimFeature_WalkingMovement::UpdatePivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.AnimSettingsPtr || !GetCharacterMovement()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;
	if (!USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator)) return;

	const bool bCanRePivot = CanRePivot();
	const bool bIsStopping = GetAnimVelocity2D().Dot(GetMovementInput2D()) < 0.f;

	/** Check for repivot in another direction or change the pivot anim at the starting's very start. */
	if (bCanRePivot || PivotStrideWarpingAlpha < 1.f)
	{
		if (UAnimSequence* NewAnim = CalcPivotAnim())
		{
			/** Different animation means direction change. */
			if (USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator) != NewAnim)
			{
				USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, NewAnim, GetPossibleInertialBlendingTime());
				PivotInputRelativeToEvaluated = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetMovementInput2D());
				// PivotVelocityRelativeToEvaluated = UKismetMathLibrary::Quat_UnrotateVector(EvaluatedTargetOrientationForInput.Quaternion(), GetAnimVelocity2D());
			}
		}
	}

	/** Reset data when repivot. */
	if (bCanRePivot)
	{
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
		PivotStrideWarpingAlpha = 0.f;
		PivotStopTime = 0.f;
	}
	
	if (!USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator)) return;
	float ExplicitTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);

	/** Distance matching to target for stop. */
	if (bIsStopping)
	{
		PivotStopTime = ExplicitTime;
		
		const FVector MovementToStopWS = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(GetMovementInput2D(), GetAnimVelocity2D(), GetCharacterMovement()->GroundFriction);
		const FVector& MovementToStopWS2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(MovementToStopWS);
		const float DistanceToStop2DRS = MovementToStopWS2DRS.Size();
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, DistanceToStop2DRS, DistanceCurveName);
	}
	/** Distance matching by distance for start. */
	else
	{
		PivotStrideWarpingAlpha = UKismetMathLibrary::MapRangeClamped(ExplicitTime - PivotStopTime, AnimData.AnimSettingsPtr->StartStrideBlendInTimeRange.X, AnimData.AnimSettingsPtr->StartStrideBlendInTimeRange.Y, 0.f, 1.f);

		const FVector& VelocityWallAdjusted2DRS = GetCharacter()->GetActorTransform().InverseTransformVector(GetAnimVelocity2D());
		const float& DeltaDistance2DRS = VelocityWallAdjusted2DRS.Size() * UAnimExecutionContextLibrary::GetDeltaTime(Context);
		const float PlayRateClampX = FMath::Lerp(0.2f, AnimData.AnimSettingsPtr->StartPlayRateRange.X, StartStrideWarpingAlpha);

		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator, DeltaDistance2DRS, DistanceCurveName, FVector2D(PlayRateClampX, AnimData.AnimSettingsPtr->StartPlayRateRange.Y));
	}
}