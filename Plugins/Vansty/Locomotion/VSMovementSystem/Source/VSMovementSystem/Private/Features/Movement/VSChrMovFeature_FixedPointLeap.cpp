// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_FixedPointLeap.h"
#include "VSChrMovCapsuleComponent.h"
#include "VSMovementSystemSettings.h"
#include "Algo/RandomShuffle.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovFeature_FixedPointLeap::UVSChrMovFeature_FixedPointLeap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
}

void UVSChrMovFeature_FixedPointLeap::OnFeatureDeactivated_Implementation()
{
	if (IsFixedPointLeapMode())
	{
		StopFixPointLeap();
	}
	
	Super::OnFeatureDeactivated_Implementation();
}

bool UVSChrMovFeature_FixedPointLeap::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsFixedPointLeapMode() && !MovementData.SnappedParams.SettingsRow.IsNull();
}

void UVSChrMovFeature_FixedPointLeap::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);
	
	const FVector& RootLocationWS = GetRootLocation();

	bool bShouldStopMovement = false;
	if (UKismetMathLibrary::InRange_FloatFloat(MovementData.MovementElapsedTime, MovementData.AnimPtr->GetMarkTime(AnimMovementStartTimeMarkName), MovementData.AnimPtr->GetMarkTime(AnimMovementEndTimeMarkName)))
	{
		FVector TargetRootPointWS = MovementData.SnappedParams.OriginalTargetRootPointWS;
		if (MovementData.CachedParams.Component.IsValid())
		{
			const FVector& OriginalTargetRootPointRS = MovementData.SnappedParams.StartComponentTransformWS.InverseTransformPosition(MovementData.SnappedParams.OriginalTargetRootPointWS);
			TargetRootPointWS = MovementData.CachedParams.Component->GetComponentTransform().TransformPosition(OriginalTargetRootPointRS);
		}
		
		const FVector& SuggestedVelocity = UVSGameplayLibrary::SuggestVelocityForProjectileMovementByTime(
			RootLocationWS,
			TargetRootPointWS,
			MovementData.AnimPtr->GetMarkTime(AnimMovementEndTimeMarkName) - MovementData.MovementElapsedTime,
			GetCharacterMovement()->GetGravityDirection(),
			FMath::Abs(GetCharacterMovement()->GetGravityZ()));
		FHitResult UpdateMovementHit;
		
		GetCharacterMovement()->Velocity = SuggestedVelocity;
		GetCharacterMovement()->SafeMoveUpdatedComponent(SuggestedVelocity * DeltaTime, GetCharacter()->GetActorRotation(), true, UpdateMovementHit);
		
		GetMovementFeatureAgent()->OrientationAimData.Direction = FVector::VectorPlaneProject(TargetRootPointWS - RootLocationWS, GetUpDirection()).GetSafeNormal();
		
		if (UpdateMovementHit.bBlockingHit)
		{
			bShouldStopMovement = true;
		}
	}
	
	MovementData.MovementElapsedTime = FMath::Clamp(MovementData.MovementElapsedTime + DeltaTime * MovementData.CachedParams.AnimPlayRate, 0.f, MovementData.AnimPtr->GetSafePlayTimeRange().Y);
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
	
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	const FGameplayTagContainer& GameplayTags = GameplayTagController->GetGameplayTags();
	
	MovementData.bMatchesEntranceTagQuery = EntranceTagQuery.Matches(TagEvent, GameplayTags);
	
	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (!IsFixedPointLeapMode() && GetPrevMovementMode() == EVSMovementMode::FixedPointLeap && !MovementData.SnappedParams.SettingsRow.IsNull())
		{
			StopFixPointLeap();
		}
	}
	else if (IsFixedPointLeapMode() && AutoBreakTagQuery.Matches(TagEvent, GameplayTags))
	{
		StopFixPointLeap();
	}
}

bool UVSChrMovFeature_FixedPointLeap::IsFixedPointLeapMode() const
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	return GameplayTagController ? (GameplayTagController->GetTagCount(EVSMovementMode::FixedPointLeap) > 0) : false;
}

void UVSChrMovFeature_FixedPointLeap::TryFixedPointLeap(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocation, AActor* ActorToFollow, FName ComponentName, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (!MovementData.bMatchesEntranceTagQuery) return;
	if (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty()) return;
	USceneComponent* ComponentToFollow = ActorToFollow ? Cast<USceneComponent>(UVSActorLibrary::GetActorComponentByName(ActorToFollow, ComponentName)) : nullptr;
	const bool bIsReplicatedComponent = ComponentToFollow && ComponentToFollow->GetOwner()->GetIsReplicated() && ComponentToFollow->IsReadyForReplication();
	const FVector& TargetRootLocationUndefinedSpace = bIsReplicatedComponent ? ComponentToFollow->GetComponentTransform().InverseTransformPosition(TargetRootLocation) : TargetRootLocation;
	
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			const bool bSucceeded = TryFixedPointLeapInternal(SettingRows, TargetRootLocationUndefinedSpace, ActorToFollow, ComponentName);
			if (bSucceeded && NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
			{
				/** Only send server RPC if local execution succeeded.  */
				FixedPointLeap_Server(MovementData.SnappedParams, NetExecPolicies.ServerRPCPolicy);
			}
		}
		else if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			TryFixedPointLeap_Server(SettingRows, TargetRootLocationUndefinedSpace, ActorToFollow, ComponentName, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_Authority)
	{
		TryFixedPointLeap_Server(SettingRows, TargetRootLocationUndefinedSpace, ActorToFollow, ComponentName, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			TryFixedPointLeapInternal(SettingRows, TargetRootLocationUndefinedSpace, ActorToFollow, ComponentName);
		}
	}
}

void UVSChrMovFeature_FixedPointLeap::StopFixPointLeap()
{
	if (!FMath::IsNearlyZero(MovementData.CapsuleHalfHeightOffsetUSCZ, 0.01f))
	{
		GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(GetMovementCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - MovementData.CapsuleHalfHeightOffsetUSCZ);
	}
	MovementData = FMovementData();
	
	if (UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()))
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
	if (IsFixedPointLeapMode())
	{
		SetMovementMode(UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()) ? EVSMovementMode::Walking : EVSMovementMode::Falling);
	}
}

bool UVSChrMovFeature_FixedPointLeap::TryFixedPointLeapInternal(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocationUndefinedSpace, AActor* ActorToFollow, FName ComponentName)
{
	if (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty()) return false;
	TArray<FDataTableRowHandle> SettingRowsToUse = !SettingRows.IsEmpty() ? SettingRows : DefaultSettingRows;

	USceneComponent* ComponentToFollow = ActorToFollow ? Cast<USceneComponent>(UVSActorLibrary::GetActorComponentByName(ActorToFollow, ComponentName)) : nullptr;
	const FVector& TargetRootLocationWS = ComponentToFollow ? ComponentToFollow->GetComponentTransform().TransformPosition(TargetRootLocationUndefinedSpace) : TargetRootLocationUndefinedSpace;

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
		
		const float TargetHalfHeightSC = Settings->CapsuleHalfHeight > 0.f ? (Settings->CapsuleHalfHeight * GetScale3D().Z) : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FVector& RootLocationWS = GetRootLocation();
		PredictProjectilePathParams.StartLocation = RootLocationWS + GetUpDirection() * TargetHalfHeightSC;

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

			MovementData.SnappedParams.SettingsRow = SettingRow;
			MovementData.SnappedParams.AnimRow = AnimRow;
			MovementData.SnappedParams.OriginalTargetRootPointWS = TargetRootLocationWS;
			MovementData.SnappedParams.ServerSideServerStartTime = UVSGameplayLibrary::GetServerTimeSeconds(this);
			if (ComponentToFollow)
			{
				MovementData.SnappedParams.Actor = ComponentToFollow->GetOwner();
				MovementData.SnappedParams.ComponentName = ComponentToFollow->GetFName();
				MovementData.SnappedParams.StartComponentTransformWS = ComponentToFollow->GetComponentTransform();
			}
			
			FixedPointLeapBySnappedParams(MovementData.SnappedParams);
			return true;
		}
	}

	return false;
}

void UVSChrMovFeature_FixedPointLeap::FixedPointLeapBySnappedParams(const FVSFixedPointLeapSnappedParams& SnappedParams)
{
	if (SnappedParams.SettingsRow.IsNull() || SnappedParams.AnimRow.IsNull()) return;
	MovementData.SnappedParams = SnappedParams;

	MovementData.SettingsPtr = MovementData.SnappedParams.SettingsRow.GetRow<FVSFixedPointLeapSettings>(nullptr);
	MovementData.AnimPtr = MovementData.SnappedParams.AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
	MovementData.MovementElapsedTime = MovementData.AnimPtr->GetSafePlayTimeRange().X;

	MovementData.CachedParams.AnimPlayRate = MovementData.AnimPtr->PlayRate;
	MovementData.CachedParams.ClientSideServerStartTime = UVSGameplayLibrary::GetServerTimeSeconds(this);
	MovementData.CachedParams.ActionID = FMath::RandRange(0, INT16_MAX);
	
	
	if (!IsFixedPointLeapMode())
	{
		SetMovementMode(EVSMovementMode::FixedPointLeap, false);
	}
	GetCharacterMovement()->StopMovementImmediately();
	const float TargetHalfHeightUSC = MovementData.SettingsPtr->CapsuleHalfHeight > 0.f ? MovementData.SettingsPtr->CapsuleHalfHeight : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MovementData.CapsuleHalfHeightOffsetUSCZ = TargetHalfHeightUSC - GetCharacter()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(TargetHalfHeightUSC);
}

void UVSChrMovFeature_FixedPointLeap::TryFixedPointLeap_Server_Implementation(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocationUndefinedSpace, AActor* ActorToFollow, FName ComponentName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		const bool bSuccessful = TryFixedPointLeapInternal(SettingRows, TargetRootLocationUndefinedSpace, ActorToFollow, ComponentName);
		if (bSuccessful && NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			FixedPointLeap_Multicast(MovementData.SnappedParams, NetExecPolicy);
		}
	}
}

void UVSChrMovFeature_FixedPointLeap::FixedPointLeap_Server_Implementation(const FVSFixedPointLeapSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		FixedPointLeapBySnappedParams(SnappedParams);
		if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			FixedPointLeap_Multicast(MovementData.SnappedParams, NetExecPolicy);
		}
	}
}

void UVSChrMovFeature_FixedPointLeap::FixedPointLeap_Multicast_Implementation(const FVSFixedPointLeapSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;
	if (SnappedParams.SettingsRow.IsNull() || SnappedParams.AnimRow.IsNull()) { bShouldExecute = false; }

	/** Authority already handled. */
	if (GetCharacter()->HasAuthority()) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		FixedPointLeapBySnappedParams(SnappedParams);
	}
}