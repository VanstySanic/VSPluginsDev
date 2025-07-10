// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_FixedPointLeap.h"
#include "VSMovementSystemSettings.h"
#include "Algo/RandomShuffle.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovFeature_FixedPointLeap::UVSChrMovFeature_FixedPointLeap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSChrMovFeature_FixedPointLeap::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UVSChrMovFeature_FixedPointLeap, ReplicatedSnappedParams, SharedParams);
}

bool UVSChrMovFeature_FixedPointLeap::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsFixedPointLeapMode() && !MovementData.SnappedParams.SettingsRow.IsNull();
}

void UVSChrMovFeature_FixedPointLeap::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	FVector TargetRootPointWS = MovementData.SnappedParams.OriginalTargetRootPointWS;
	if (MovementData.SnappedParams.Component.IsValid())
	{
		const FVector& OriginalTargetRootPointRS = MovementData.SnappedParams.StartComponentTransformWS.InverseTransformPosition(MovementData.SnappedParams.OriginalTargetRootPointWS);
		TargetRootPointWS = MovementData.SnappedParams.Component->GetComponentTransform().TransformPosition(OriginalTargetRootPointRS);
	};

	const FVector& RootLocationWS = UVSActorLibrary::GetCharacterRootLocation(GetCharacter());

	bool bShouldStopMovement = false;
	if (UKismetMathLibrary::InRange_FloatFloat(MovementData.MovementElapsedTime, MovementData.AnimPtr->GetMarkTime(AnimMovementStartTimeMarkName), MovementData.AnimPtr->GetMarkTime(AnimMovementEndTimeMarkName)))
	{
		const FVector& SuggestedVelocity = UVSGameplayLibrary::SuggestVelocityForProjectileMovementByTime(
			RootLocationWS,
			TargetRootPointWS,
			MovementData.AnimPtr->GetMarkTime(AnimMovementEndTimeMarkName) - MovementData.MovementElapsedTime,
			GetCharacterMovement()->GetGravityDirection(),
			FMath::Abs(GetCharacterMovement()->GetGravityZ()));
		FHitResult UpdateMovementHit;
		GetCharacterMovement()->Velocity = SuggestedVelocity;
		GetCharacterMovement()->SafeMoveUpdatedComponent(SuggestedVelocity * DeltaTime, GetCharacter()->GetActorRotation(), true, UpdateMovementHit);
		
		if (UpdateMovementHit.bBlockingHit)
		{
			bShouldStopMovement = true;
		}
	}
	
	if (UVSChrMovFeature_OrientationEvaluator_Common* Evaluator = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_OrientationEvaluator_Common>())
	{
		if (!TargetRootPointWS.Equals(RootLocationWS))
		{
			Evaluator->DefaultNamedParams.VectorParams.Emplace(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.AimTargetPoint, GetCharacter()->GetActorLocation() + FVector::VectorPlaneProject(TargetRootPointWS - RootLocationWS, GetUpDirection()).GetSafeNormal() * 16.f);

		}
	}

	MovementData.MovementElapsedTime = FMath::Clamp(MovementData.MovementElapsedTime + DeltaTime * MovementData.AnimPtr->PlayRate, 0.f, MovementData.AnimPtr->GetSafePlayTimeRange().Y);
	if (MovementData.MovementElapsedTime >= MovementData.AnimPtr->GetSafePlayTimeRange().Y)
	{
		bShouldStopMovement = true;
	}
	if (MovementData.MovementElapsedTime > MovementData.AnimPtr->GetMarkTime(AnimMovementEndTimeMarkName) && HasMovementInput2D())
	{
		bShouldStopMovement = true;
	}

	if (bShouldStopMovement)
	{
		StopFixPointLeap();
	}
}

void UVSChrMovFeature_FixedPointLeap::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (!IsFixedPointLeapMode() && GetPrevMovementMode() == EVSMovementMode::FixedPointLeap && !MovementData.SnappedParams.SettingsRow.IsNull())
		{
			StopFixPointLeap();
		}
	}
}

bool UVSChrMovFeature_FixedPointLeap::IsFixedPointLeapMode() const
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	return GameplayTagController ? (GameplayTagController->GetTagCount(EVSMovementMode::FixedPointLeap) > 0) : false;
}

void UVSChrMovFeature_FixedPointLeap::TryFixedPointLeap(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocation, USceneComponent* ComponentToFollow)
{
	if (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty()) return;
	const bool bIsReplicatedComponent = ComponentToFollow && ComponentToFollow->GetOwner()->GetIsReplicated() && ComponentToFollow->IsReadyForReplication();
	const FVector& TargetRootLocationUndefinedSpace = bIsReplicatedComponent ? ComponentToFollow->GetComponentTransform().InverseTransformPosition(TargetRootLocation) : TargetRootLocation;
	
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		TryFixedPointLeap_Server(SettingRows, TargetRootLocationUndefinedSpace, bIsReplicatedComponent ? ComponentToFollow : nullptr);
	}
	else
	{
		TryFixedPointLeapInternal(SettingRows, TargetRootLocationUndefinedSpace, ComponentToFollow);
	}
}

void UVSChrMovFeature_FixedPointLeap::StopFixPointLeap()
{
	if (IsFixedPointLeapMode())
	{
		SetMovementMode(UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()) ? EVSMovementMode::Walking : EVSMovementMode::Falling);
	}
	
	if (UVSChrMovFeature_OrientationEvaluator* Evaluator = GetMovementFeatureAgent()->FindSubFeatureByClass<UVSChrMovFeature_OrientationEvaluator>())
	{
		Evaluator->DefaultNamedParams.VectorParams.Remove(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.AimTargetPoint);
	}
	
	MovementData = FMovementData();
	ReplicatedSnappedParams = FVSFixedPointLeapSnappedParams();
}

void UVSChrMovFeature_FixedPointLeap::TryFixedPointLeapInternal(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocationUndefinedSpace, USceneComponent* ComponentToFollow)
{
	if (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty()) return;
	TArray<FDataTableRowHandle> SettingRowsToUse = !SettingRows.IsEmpty() ? SettingRows : DefaultSettingRows;

	const FVector& TargetRootLocationWS = ComponentToFollow ? ComponentToFollow->GetComponentTransform().TransformPosition(TargetRootLocationUndefinedSpace) : TargetRootLocationUndefinedSpace;
	const FVector& RootLocationWS = UVSActorLibrary::GetCharacterRootLocation(GetCharacter());

	FPredictProjectilePathParams PredictProjectilePathParams;
	PredictProjectilePathParams.MaxSimTime = 6.4f;
	PredictProjectilePathParams.ProjectileRadius = GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	PredictProjectilePathParams.bTraceWithChannel = true;
	PredictProjectilePathParams.TraceChannel = ECC_Pawn;
	PredictProjectilePathParams.ActorsToIgnore.Emplace(GetCharacter());
	PredictProjectilePathParams.bTraceWithCollision = true;
	FPredictProjectilePathResult PredictProjectilePathResult;
	
	for (const FDataTableRowHandle& SettingRow : SettingRowsToUse)
	{
		FVSFixedPointLeapSettings* Settings = SettingRow.GetRow<FVSFixedPointLeapSettings>(nullptr);
		if (!Settings || !Settings->IsValid()) continue;
		if (!Settings->Limits.Matches(GetMovementFeatureAgent())) continue;

		TArray<FDataTableRowHandle> AvailableAnimRows;
		for (const FDataTableRowHandle& AnimRow : Settings->AnimRows)
		{
			FVSAnimSequenceReference* Anim = AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
			if (!Anim || !Anim->IsValid()) continue;
			if (!Anim->HasTimeMark(AnimMovementStartTimeMarkName) || !Anim->HasTimeMark(AnimMovementEndTimeMarkName)) continue;
			if (Anim->GetMarkTime(AnimMovementStartTimeMarkName) >= Anim->GetMarkTime(AnimMovementEndTimeMarkName)) continue;
			AvailableAnimRows.Add(AnimRow);
		}
		if (AvailableAnimRows.IsEmpty()) continue;
		Algo::RandomShuffle(AvailableAnimRows);

		const float TargetHalfHeight = Settings->CapsuleHalfHeight > 0.f ? (Settings->CapsuleHalfHeight * GetScale3D().Z) : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		PredictProjectilePathParams.StartLocation = RootLocationWS + GetUpDirection() * TargetHalfHeight;

		for (const FDataTableRowHandle& AnimRow : AvailableAnimRows)
		{
			FVSAnimSequenceReference* Anim = AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
			const float MovementTime = Anim->GetMarkTime(AnimMovementEndTimeMarkName) - Anim->GetMarkTime(AnimMovementStartTimeMarkName);
			const FVector& SuggestedVelocity = UVSGameplayLibrary::SuggestVelocityForProjectileMovementByTime(RootLocationWS, TargetRootLocationWS, MovementTime, GetGravityDirection(), FMath::Abs(GetCharacterMovement()->GetGravityZ()));

			PredictProjectilePathParams.LaunchVelocity = SuggestedVelocity;
			if (!UGameplayStatics::PredictProjectilePath(this, PredictProjectilePathParams, PredictProjectilePathResult)) continue;
			if (PredictProjectilePathResult.HitResult.bBlockingHit)
			{
				if (!GetCharacterMovement()->IsWalkable(PredictProjectilePathResult.HitResult) || !PredictProjectilePathResult.HitResult.ImpactPoint.Equals(PredictProjectilePathResult.HitResult.ImpactPoint, 8.f)) continue;
			}
			
			/** This is a valid process. */
			MovementData.MovementElapsedTime = 0.f;
			
			MovementData.SnappedParams.Component = ComponentToFollow;
			MovementData.SnappedParams.SettingsRow = SettingRow;
			MovementData.SnappedParams.AnimRow = AnimRow;
			MovementData.SnappedParams.OriginalTargetRootPointWS = TargetRootLocationWS;
			MovementData.SnappedParams.StartComponentTransformWS = ComponentToFollow ? ComponentToFollow->GetComponentTransform() : FTransform::Identity;

			FixedPointLeapBySnappedParams(MovementData.SnappedParams);
			return;
		}
	}
}

void UVSChrMovFeature_FixedPointLeap::FixedPointLeapBySnappedParams(const FVSFixedPointLeapSnappedParams& SnappedParams)
{
	if (SnappedParams.SettingsRow.IsNull() || SnappedParams.AnimRow.IsNull()) return;
	MovementData.SnappedParams = SnappedParams;

	MovementData.SettingsPtr = MovementData.SnappedParams.SettingsRow.GetRow<FVSFixedPointLeapSettings>(nullptr);
	MovementData.AnimPtr = MovementData.SnappedParams.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	MovementData.MovementElapsedTime = MovementData.AnimPtr->GetSafePlayTimeRange().X;
	
	MovementData.SnappedParams.ActionID = FMath::RandRange(0, INT16_MAX);
	
	const float TargetHalfHeight = MovementData.SettingsPtr->CapsuleHalfHeight > 0.f ? (MovementData.SettingsPtr->CapsuleHalfHeight * GetScale3D().Z) : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacter()->GetCapsuleComponent()->SetCapsuleHalfHeight(TargetHalfHeight);
	if (!IsFixedPointLeapMode())
	{
		SetMovementMode(EVSMovementMode::FixedPointLeap);
	}

	if (GetCharacter()->HasAuthority() && GetIsReplicated())
	{
		ReplicatedSnappedParams = MovementData.SnappedParams;
		MARK_PROPERTY_DIRTY_FROM_NAME(UVSChrMovFeature_FixedPointLeap, ReplicatedSnappedParams, this);
	}
}

void UVSChrMovFeature_FixedPointLeap::TryFixedPointLeap_Server_Implementation(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocationUndefinedSpace, USceneComponent* ComponentToFollow)
{
	TryFixedPointLeapInternal(SettingRows, TargetRootLocationUndefinedSpace, ComponentToFollow);
}

void UVSChrMovFeature_FixedPointLeap::OnRep_ReplicatedSnappedParams()
{
	if (ReplicatedSnappedParams.SettingsRow.IsNull() || ReplicatedSnappedParams.AnimRow.IsNull()) return;
	FixedPointLeapBySnappedParams(ReplicatedSnappedParams);
}
