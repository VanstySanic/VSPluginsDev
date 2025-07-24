// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovAnimFeature_FallingMovement.h"
#include "AnimDistanceMatchingLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/VSChrMovAnimInAirTypes.h"
#include "Types/VSMovementAnimTags.h"
#include "Types/Animation/VSAnimSequenceReference.h"

void UVSChrMovAnimFeature_FallingMovement::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	UpdateMovementTagQueryStates(FGameplayTag::EmptyTag);
}

void UVSChrMovAnimFeature_FallingMovement::UpdateAnimation_Implementation(float DeltaTime)
{
	Super::UpdateAnimation_Implementation(DeltaTime);

	if (!GetCharacter() || !GetCharacterMovement()) return;
	
	DistanceToLandScaled = 65536.f;
	bIsFallingDown = GetVelocityZ().Dot(GetUpDirection()) < 0.f;
	bIsMovingUp = GetVelocityZ().Dot(GetUpDirection()) > 0.f;
	if (bIsFallingDown)
	{
		const FVector& Start = GetCharacter()->GetActorLocation();
		const FVector& End = Start + 1.2f * AnimData.DistanceToLandThreshold * UKismetMathLibrary::SafeDivide(GetVelocity().Size(), GetVelocityZ().Size()) * GetVelocity().GetSafeNormal();

		const FCollisionShape& TraceShape = GetCharacter()->GetCapsuleComponent()->GetCollisionShape();
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(GetCharacter());
		FHitResult HitResult;

		/** Trace for a landing position and calculate the distance. */
		GetCharacter()->GetWorld()->SweepSingleByChannel(
				HitResult,
				Start,
				End,
				FQuat::Identity,
				ECC_Pawn,
				TraceShape,
				TraceParams);
		
		if (HitResult.IsValidBlockingHit())
		{
			DistanceToLandScaled = UKismetMathLibrary::Vector_ProjectOnToNormal(GetCharacter()->GetActorLocation() - HitResult.Location, GetUpDirection()).Z;
			DistanceToLandScaled = UKismetMathLibrary::SafeDivide(DistanceToLandScaled, GetCharacter()->GetActorScale().Z);
		}
	}
}

void UVSChrMovAnimFeature_FallingMovement::UpdateAnimationThreadSafe_Implementation(float DeltaTime)
{
	Super::UpdateAnimationThreadSafe_Implementation(DeltaTime);

	bMovementReassignedThisFrame = (bool)AnimData.AnimSettingsReassignedMark;
	AnimData.AnimSettingsReassignedMark = false;

	OrientationWarpingAngle = CalcOrientationWarpingAngle();
}

bool UVSChrMovAnimFeature_FallingMovement::HasPossibleStartAnim() const
{
	if (!AnimData.CurrentAnimSettingsPtr) return false;
	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	return StartAnim && StartAnim->IsValid();
}

bool UVSChrMovAnimFeature_FallingMovement::HasPossibleStartLoopAnim() const
{
	if (!AnimData.CurrentAnimSettingsPtr) return false;
	if (AnimData.CurrentAnimSettingsPtr->StartLoopAnim) return true;
	FVSAnimSequenceReference* Anim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	return Anim && Anim->IsValid() && Anim->HasTimePeriod(StartAnimLoopSwingTimePeriodName);
}

bool UVSChrMovAnimFeature_FallingMovement::HasPossibleApexAnim() const
{
	if (!AnimData.CurrentAnimSettingsPtr) return false;
	if (AnimData.CurrentAnimSettingsPtr->StartLoopAnim) return true;
	FVSAnimSequenceReference* Anim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	return Anim && Anim->IsValid() && Anim->HasTimeMark(StartAnimApexTimeMarkName);
}

bool UVSChrMovAnimFeature_FallingMovement::HasPossibleLandAnim() const
{
	if (!AnimData.CurrentAnimSettingsPtr) return false;
	FVSAnimSequenceReference* LandAnim = AnimData.CurrentAnimSettingsPtr->LandAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	return LandAnim && LandAnim->IsValid();
}

bool UVSChrMovAnimFeature_FallingMovement::CanExitStartStateNaturally() const
{
	if (!HasPossibleStartAnim()) return true;

	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	
	const bool bStartAsLoop = !AnimData.CurrentAnimSettingsPtr->StartLoopAnim && StartAnim->HasTimePeriod(StartAnimLoopSwingTimePeriodName);
	const bool bStartAsApex = !AnimData.CurrentAnimSettingsPtr->ApexAnim && StartAnim->HasTimeMark(StartAnimApexTimeMarkName);
	if (!bStartAsLoop && !bStartAsApex && AnimData.StartAnimPlayedTime >= StartAnim->GetSafePlayTimeRange().Y) return true;
	if (bStartAsLoop && AnimData.StartAnimPlayedTime >= StartAnim->GetPeriodTimeRange(StartAnimLoopSwingTimePeriodName).X) return true;
	if (!bStartAsLoop && bStartAsApex && AnimData.StartAnimPlayedTime >= StartAnim->GetMarkTime(StartAnimApexTimeMarkName)) return true;

	return false;
}

bool UVSChrMovAnimFeature_FallingMovement::CanEnterLandStateNaturally() const
{
	if (!HasPossibleLandAnim()) return false;
	if (GetVelocityZ().Dot(GetUpDirection()) > 0.f) return false;
	
	return DistanceToLandScaled <= AnimData.DistanceToLandThreshold;
}

float UVSChrMovAnimFeature_FallingMovement::CalcOrientationWarpingAngle() const
{
	const FGameplayTag& VelocityDirectionToCharacter = GetAnimVelocityDirectionToCharacter2D();
	if (VelocityDirectionToCharacter == EVSAnimDirection::XX) return 0.f;
	
	const FVector& Velocity2DDS = FVector::VectorPlaneProject(GetAnimVelocity2D(), GetUpDirection());
	const FVector& Forward2DDS = FVector::VectorPlaneProject(GetCharacter()->GetActorForwardVector(), GetUpDirection());
	const FQuat& DeltaRotation2DDS = FQuat::FindBetweenVectors(Forward2DDS, Velocity2DDS);
	const float Sign = FMath::Sign(Forward2DDS.Cross(Velocity2DDS).Dot(GetUpDirection()));
	const float VelocityAngle2DDS = Sign * FMath::RadiansToDegrees(DeltaRotation2DDS.GetAngle());

	static TArray<FGameplayTag> DirectAngle = TArray<FGameplayTag>
	{
		EVSAnimDirection::FX,
		EVSAnimDirection::FL,
		EVSAnimDirection::FR,
		EVSAnimDirection::XR
	};
	
	static TArray<FGameplayTag> InverseAngle = TArray<FGameplayTag>
	{
		EVSAnimDirection::BX,
		EVSAnimDirection::BL,
		EVSAnimDirection::BR,
		EVSAnimDirection::XL
	};
	
	if (DirectAngle.Contains(VelocityDirectionToCharacter))
	{
		return VelocityAngle2DDS;
	}
	if (InverseAngle.Contains(VelocityDirectionToCharacter))
	{
		return -Sign * (180.f - FMath::Abs(VelocityAngle2DDS));
	}
	
	return 0.f;
}

FVector UVSChrMovAnimFeature_FallingMovement::CalcOrientationWarpingDirection() const
{
	const FGameplayTag& VelocityDirectionToCharacter = GetAnimVelocityDirectionToCharacter2D();
	if (VelocityDirectionToCharacter == EVSAnimDirection::XX) return FVector::ZeroVector;
	
	const FVector& Velocity2DDS = FVector::VectorPlaneProject(GetAnimVelocity2D(), GetUpDirection());
	const FVector& Forward2DDS = FVector::VectorPlaneProject(GetCharacter()->GetActorForwardVector(), GetUpDirection());
	const FQuat& DeltaRotation2DDS = FQuat::FindBetweenVectors(Forward2DDS, Velocity2DDS);
	const float Sign = FMath::Sign(Forward2DDS.Cross(Velocity2DDS).Dot(GetUpDirection()));
	const float VelocityAngle2DDS = Sign * FMath::RadiansToDegrees(DeltaRotation2DDS.GetAngle());

	static TArray<FGameplayTag> DirectAngle = TArray<FGameplayTag>
	{
		EVSAnimDirection::FX,
		EVSAnimDirection::FL,
		EVSAnimDirection::FR,
		EVSAnimDirection::XR
	};
	
	static TArray<FGameplayTag> InverseAngle = TArray<FGameplayTag>
	{
		EVSAnimDirection::BX,
		EVSAnimDirection::BL,
		EVSAnimDirection::BR,
		EVSAnimDirection::XL
	};
	
	if (DirectAngle.Contains(VelocityDirectionToCharacter))
	{
		return GetAnimVelocity2D();
	}
	if (InverseAngle.Contains(VelocityDirectionToCharacter))
	{
		return -GetAnimVelocity2D();
	}
	
	return FVector::ZeroVector;
}

void UVSChrMovAnimFeature_FallingMovement::SetupStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleStartAnim()) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	if (UAnimSequenceBase* NewAnim = StartAnim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, StartAnim->GetSafePlayTimeRange().X);
	AnimData.StartAnimPlayedTime = StartAnim->GetSafePlayTimeRange().X;
}

void UVSChrMovAnimFeature_FallingMovement::UpdateStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleStartAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	/** Check for new movement process. */
	if (bMovementReassignedThisFrame)
	{
		if (UAnimSequenceBase* NewAnim = StartAnim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, StartAnim->GetSafePlayTimeRange().X);
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
		AnimData.StartAnimPlayedTime = StartAnim->GetSafePlayTimeRange().X;
	}
	
	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, StartAnim->PlayRate);
	AnimData.StartAnimPlayedTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);
}

void UVSChrMovAnimFeature_FallingMovement::SetupStartLoopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleStartLoopAnim()) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;
	
	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	if (UAnimSequenceBase* NewAnim = AnimData.CurrentAnimSettingsPtr->StartLoopAnim ? AnimData.CurrentAnimSettingsPtr->StartLoopAnim : StartAnim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, AnimData.CurrentAnimSettingsPtr->StartLoopAnim ? 0.f : StartAnim->GetPeriodTimeRange(StartAnimLoopSwingTimePeriodName).X);
}

void UVSChrMovAnimFeature_FallingMovement::UpdateStartLoopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleStartLoopAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	/** Check for new movement process. */
	if (bMovementReassignedThisFrame)
	{
		if (UAnimSequenceBase* NewAnim = AnimData.CurrentAnimSettingsPtr->StartLoopAnim ? AnimData.CurrentAnimSettingsPtr->StartLoopAnim : StartAnim->AnimSequence)
		{
			if (NewAnim != USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator))
			{
				USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, NewAnim);
			}
			USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, AnimData.CurrentAnimSettingsPtr->StartLoopAnim ? 0.f : StartAnim->GetPeriodTimeRange(StartAnimLoopSwingTimePeriodName).X);
		}
	}

	if (AnimData.CurrentAnimSettingsPtr->StartLoopAnim)
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
	}
	else
	{
		const FVector2D& Period = StartAnim->GetPeriodTimeRange(StartAnimLoopSwingTimePeriodName);
		float NewTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator) + Context.GetContext()->GetDeltaTime() * StartAnim->PlayRate;
		NewTime = UKismetMathLibrary::Wrap(NewTime, Period.X, Period.Y);
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, NewTime);
	}
}

void UVSChrMovAnimFeature_FallingMovement::SetupApexAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleApexAnim()) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	if (UAnimSequenceBase* NewAnim = AnimData.CurrentAnimSettingsPtr->ApexAnim ? AnimData.CurrentAnimSettingsPtr->ApexAnim : StartAnim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, AnimData.CurrentAnimSettingsPtr->ApexAnim ? 0.f : StartAnim->GetMarkTime(StartAnimApexTimeMarkName));
}

void UVSChrMovAnimFeature_FallingMovement::UpdateApexAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleApexAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* StartAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	/** Check for new movement process. */
	if (bMovementReassignedThisFrame)
	{
		if (UAnimSequenceBase* NewAnim = AnimData.CurrentAnimSettingsPtr->ApexAnim ? AnimData.CurrentAnimSettingsPtr->ApexAnim : StartAnim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, AnimData.CurrentAnimSettingsPtr->ApexAnim ? 0.f : StartAnim->GetMarkTime(StartAnimApexTimeMarkName));
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
	}

	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, AnimData.CurrentAnimSettingsPtr->ApexAnim ? 1.f : StartAnim->PlayRate);
}

void UVSChrMovAnimFeature_FallingMovement::UpdateFallAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!AnimData.CurrentAnimSettingsPtr) return;
	
	FSequencePlayerReference SequencePlayer;
	bool bResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, bResult);
	if (!bResult) return;

	if (UAnimSequenceBase* NewAnim = AnimData.CurrentAnimSettingsPtr->FallAnim)
	{
		if (NewAnim != USequencePlayerLibrary::GetSequencePure(SequencePlayer))
		{
			USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, NewAnim);
		}
	}
}

void UVSChrMovAnimFeature_FallingMovement::SetupLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleLandAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* LandAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	if (UAnimSequenceBase* NewAnim = LandAnim->AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
	}

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, LandAnim->GetSafePlayTimeRange().X);
}

void UVSChrMovAnimFeature_FallingMovement::UpdateLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!HasPossibleLandAnim()) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool bResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, bResult);
	if (!bResult) return;

	FVSAnimSequenceReference* LandAnim = AnimData.CurrentAnimSettingsPtr->StartAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	/** Check for new movement process. */
	if (bMovementReassignedThisFrame)
	{
		if (UAnimSequenceBase* NewAnim = LandAnim->AnimSequence)
		{
			USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, NewAnim);
		}
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, LandAnim->GetSafePlayTimeRange().X);
		UVSAnimationLibrary::SetInterialBlendingForSequenceEvaluator(Context, SequenceEvaluator);
	}

	if (UVSAnimationLibrary::AnimationHasCurve(LandAnim->AnimSequence, DistanceToLandCurveName) && DistanceToLandScaled > 0.01f)
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, DistanceToLandScaled, DistanceToLandCurveName);
	}
	else
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, LandAnim->PlayRate);
	}
}

void UVSChrMovAnimFeature_FallingMovement::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	UpdateMovementTagQueryStates(TagEvent);
}

void UVSChrMovAnimFeature_FallingMovement::UpdateMovementTagQueryStates(const FGameplayTag& TagEvent)
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	FGameplayTagContainer GameplayTags;
	GameplayTagController->GetOwnedGameplayTags(GameplayTags);
	UVSCharacterMovementAnimFeatureAgent* AnimFeatureAgent = GetAnimFeatureAgent();

	/** Reset movement data when out of falling mode. */
	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (GetPrevMovementMode() == EVSMovementMode::Falling)
		{
			bIsFallingDown = GetVelocityZ().Dot(GetUpDirection()) < 0.f;
			bIsMovingUp = GetVelocityZ().Dot(GetUpDirection()) > 0.f;
		}
	}
	/** Refresh movement data when enter falling mode. */
	if (GetMovementMode() == EVSMovementMode::Falling && (GetPrevMovementMode() != EVSMovementMode::Falling || ReassignAnimSettingsQuery.Matches(GameplayTags, TagEvent)))
	{
		FDataTableRowHandle SettingsRowToUse = DefaultSettingsRow;
		for (const FDataTableRowHandle& AnimSettingRow : AnimSettingRows)
		{
			FVSFallingMovementAnimSettings* Settings = AnimSettingRow.GetRow<FVSFallingMovementAnimSettings>(nullptr);
			if (!Settings || !Settings->IsValid()) continue;
			if (!Settings->Limits.Matches(AnimFeatureAgent)) continue;
			SettingsRowToUse = AnimSettingRow;
			break;
		}

		AnimData.CurrentAnimSettingsPtr = SettingsRowToUse.GetRow<FVSFallingMovementAnimSettings>(nullptr);
		if (AnimData.CurrentAnimSettingsPtr && AnimData.CurrentAnimSettingsPtr->IsValid())
		{
			FVSAnimSequenceReference* Anim = AnimData.CurrentAnimSettingsPtr->LandAnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
			if (Anim && Anim->IsValid() && UVSAnimationLibrary::AnimationHasCurve(Anim->AnimSequence, DistanceToLandCurveName))
			{
				const float StartCurve = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, DistanceToLandCurveName, Anim->GetSafePlayTimeRange().X);
				const float EndCurve = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, DistanceToLandCurveName, Anim->GetSafePlayTimeRange().Y);
				AnimData.DistanceToLandThreshold = FMath::Abs(EndCurve - StartCurve);
				AnimData.StartAnimPlayedTime = Anim->GetSafePlayTimeRange().X;
			}
		}
			
		AnimData.AnimSettingsReassignedMark = true;
		bMovementReassignedThisFrame = true;
	}
}
