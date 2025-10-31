// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_MantleVaultMovement.h"
#include "KismetTraceUtils.h"
#include "VSChrMovCapsuleComponent.h"
#include "VSMovementSystemSettings.h"
#include "Algo/RandomShuffle.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSAnimationLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Libraries/VSMathLibrary.h"
#include "Types/VSCharacterMovementTags.h"

UVSChrMovFeature_MantleVaultMovement::UVSChrMovFeature_MantleVaultMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool UVSChrMovFeature_MantleVaultMovement::IsMantlingOrVaultingMode() const
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	return GameplayTagController ? (GameplayTagController->GetTagCount(EVSMovementMode::MantlingOrVaulting) >= 1) : false;
}

void UVSChrMovFeature_MantleVaultMovement::TryMantleVault(const TArray<FDataTableRowHandle>& SettingRows, const FVector& WallTraceDirection, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (!MovementData.bMatchesEntranceTagQuery) return;
	if (SupportedMovementType == EVSMantleVaultMovementType::None || (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty())) return;
	
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			const bool bSucceeded = TryMantleVaultInternal(SettingRows, WallTraceDirection, SupportedMovementType);
			if (bSucceeded && NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
			{
				/** Only send server RPC if local execution succeeded.  */
				MantleVault_Server(MovementData.SnappedParams, NetExecPolicies.ServerRPCPolicy);
			}
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			TryMantleVault_Server(SettingRows, WallTraceDirection, SupportedMovementType, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_Authority)
	{
		TryMantleVault_Server(SettingRows, WallTraceDirection, SupportedMovementType, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			TryMantleVaultInternal(SettingRows, WallTraceDirection, SupportedMovementType);
		}
	}
}

void UVSChrMovFeature_MantleVaultMovement::StopMantleVault()
{
	if (UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()))
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
	if (!FMath::IsNearlyZero(MovementData.CapsuleHalfHeightOffsetUSCZ, 0.01f))
	{
		GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(GetMovementCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - MovementData.CapsuleHalfHeightOffsetUSCZ);
		MovementData.CapsuleHalfHeightOffsetUSCZ = 0.f;
	}
	MovementData = FMovementData();

	if (IsMantlingOrVaultingMode())
	{
		SetMovementMode(UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()) ? EVSMovementMode::Walking : EVSMovementMode::Falling, false);
	}
}

void UVSChrMovFeature_MantleVaultMovement::OnFeatureDeactivated_Implementation()
{
	if (IsMantlingOrVaultingMode())
	{
		StopMantleVault();
	}
	
	Super::OnFeatureDeactivated_Implementation();
}

bool UVSChrMovFeature_MantleVaultMovement::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsMantlingOrVaultingMode() && MovementData.SnappedParams.MovementType != EVSMantleVaultMovementType::None;
}

void UVSChrMovFeature_MantleVaultMovement::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);
	
	const FTransform& ComponentTransformWS = MovementData.CachedParams.Component->GetComponentTransform();

	/** Update the movement by stage. */
	const FVector& UpVectorRS = ComponentTransformWS.InverseTransformVectorNoScale(GetUpDirection());
	const FVector& UpVectorUnitRS = ComponentTransformWS.InverseTransformVector(GetUpDirection() * GetScale3D().Z);
	const FVector& MovementDirectionWS = ComponentTransformWS.TransformVectorNoScale(MovementData.SnappedParams.MovementDirection2DRS);
	const FVector& MovementDirectionUnitRS = ComponentTransformWS.InverseTransformVector(MovementDirectionWS * GetScale3D().X);
	
	const float ForwardMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.MovementElapsedTime);
	const float UpMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.MovementElapsedTime);
	
	FVector TargetRootLocationRS = ComponentTransformWS.InverseTransformPosition(GetRootLocation());
	if (MovementData.MovementElapsedTime <= MovementData.AnimPtr->GetMarkTime(AnimReachTargetTimeMarkName))
	{
		if (MovementData.AnimPtr->HasTimeMark(AnimScaleMovementToReachTargetTimeMarkName) && MovementData.AnimPtr->GetMarkTime(AnimScaleMovementToReachTargetTimeMarkName) >= MovementData.AnimPtr->GetMarkTime(AnimReachTargetTimeMarkName))
		{
			if (MovementData.MovementElapsedTime <= MovementData.AnimPtr->GetMarkTime(AnimScaleMovementToReachTargetTimeMarkName))
			{
				TargetRootLocationRS = MovementData.CachedParams.StartRootLocationRS;
				TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.X) * MovementDirectionUnitRS;
				TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.Y) * UpVectorUnitRS;
			}
			else
			{
				TargetRootLocationRS = MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS;
				if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimReachTargetMovementValues.X - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS.X))
				{
					TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS.X) / (MovementData.CachedParams.AnimReachTargetMovementValues.X - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS.X) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.CachedParams.ReachTargetRootLocationRS - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS, MovementData.SnappedParams.MovementDirection2DRS);
				}
				if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimReachTargetMovementValues.Y - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS.Y))
				{
					TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS.Y) / (MovementData.CachedParams.AnimReachTargetMovementValues.Y - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS.Y) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.CachedParams.ReachTargetRootLocationRS - MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS, UpVectorRS);
				}
			}
			
		}
		else if (!MovementData.AnimPtr->HasTimeMark(AnimScaleMovementToReachTargetTimeMarkName))
		{
			TargetRootLocationRS = MovementData.CachedParams.StartRootLocationRS;
			if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimReachTargetMovementValues.X - MovementData.CachedParams.AnimStartMovementCurveValues.X))
			{
				TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.X) / (MovementData.CachedParams.AnimReachTargetMovementValues.X - MovementData.CachedParams.AnimStartMovementCurveValues.X) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.CachedParams.ReachTargetRootLocationRS - MovementData.CachedParams.StartRootLocationRS, MovementData.SnappedParams.MovementDirection2DRS);
			}
			if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimReachTargetMovementValues.Y - MovementData.CachedParams.AnimStartMovementCurveValues.Y))
			{
				TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.Y) / (MovementData.CachedParams.AnimReachTargetMovementValues.Y - MovementData.CachedParams.AnimStartMovementCurveValues.Y) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.CachedParams.ReachTargetRootLocationRS - MovementData.CachedParams.StartRootLocationRS, UpVectorRS);
			}
		}
		else
		{
			TargetRootLocationRS = MovementData.CachedParams.StartRootLocationRS;
			TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.X) * MovementDirectionUnitRS;
			TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.Y) * UpVectorUnitRS;
		}
	}
	else if (MovementData.MovementElapsedTime <= MovementData.AnimPtr->GetMarkTime(AnimGroundPivotTimeMarkName))
	{
		TargetRootLocationRS = MovementData.CachedParams.ReachTargetRootLocationRS;
		if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimGroundPivotMovementValues.X - MovementData.CachedParams.AnimReachTargetMovementValues.X))
		{
			TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimReachTargetMovementValues.X) / (MovementData.CachedParams.AnimGroundPivotMovementValues.X - MovementData.CachedParams.AnimReachTargetMovementValues.X) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.SnappedParams.GroundPivotPointRS - MovementData.CachedParams.ReachTargetRootLocationRS, MovementData.SnappedParams.MovementDirection2DRS);
		}
		if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimGroundPivotMovementValues.Y - MovementData.CachedParams.AnimReachTargetMovementValues.Y))
		{
			TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimReachTargetMovementValues.Y) / (MovementData.CachedParams.AnimGroundPivotMovementValues.Y - MovementData.CachedParams.AnimReachTargetMovementValues.Y) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.SnappedParams.GroundPivotPointRS - MovementData.CachedParams.ReachTargetRootLocationRS, UpVectorRS);
		}
	}
	else
	{
		// if (MovementData.SnappedParams.MovementType != EVSMantleVaultMovementType::Vault)
		{
			TargetRootLocationRS = MovementData.SnappedParams.GroundPivotPointRS;
			TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimGroundPivotMovementValues.X) * MovementDirectionUnitRS;
			TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimGroundPivotMovementValues.Y) * UpVectorUnitRS;
		}
		// else
		// {
		// 	if (MovementData.MovementElapsedTime <= MovementData.AnimPtr->GetMarkTime(AnimVaultOffPlatformTimeMarkName))
		// 	{
		// 		TargetRootLocationRS = MovementData.SnappedParams.GroundPivotPointRS;
		// 		if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.X - MovementData.CachedParams.AnimGroundPivotMovementValues.X))
		// 		{
		// 			TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimGroundPivotMovementValues.X) / (MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.X - MovementData.CachedParams.AnimGroundPivotMovementValues.X) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.SnappedParams.VaultOffPlatformPointRS - MovementData.SnappedParams.GroundPivotPointRS, MovementData.SnappedParams.MovementDirection2DRS);
		// 		}
		// 		if (!FMath::IsNearlyZero(MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.Y - MovementData.CachedParams.AnimGroundPivotMovementValues.Y))
		// 		{
		// 			TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimGroundPivotMovementValues.Y) / (MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.Y - MovementData.CachedParams.AnimGroundPivotMovementValues.Y) * UKismetMathLibrary::ProjectVectorOnToVector(MovementData.SnappedParams.VaultOffPlatformPointRS - MovementData.SnappedParams.GroundPivotPointRS, UpVectorRS);
		// 		}
		// 	}
		// 	else
		// 	{
		// 		TargetRootLocationRS = MovementData.SnappedParams.VaultOffPlatformPointRS;
		// 		TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.X) * MovementDirectionUnitRS;
		// 		TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.Y) * UpVectorUnitRS;
		// 	}
		// }
	}

	/** Update character world space location. */
	const FVector& TargetRootLocationWS = UKismetMathLibrary::TransformLocation(ComponentTransformWS, TargetRootLocationRS);
	
	/** Update the velocity in the character movement component. */
	const FVector& DeltaRootLocationRS = TargetRootLocationRS - MovementData.LastUpdatedRootLocationRS;
	if (!DeltaRootLocationRS.ContainsNaN())
	{
		FHitResult UpdateMovementHitResult;
		GetCharacterMovement()->SafeMoveUpdatedComponent(ComponentTransformWS.TransformVector(DeltaRootLocationRS), GetCharacter()->GetActorRotation(), false, UpdateMovementHitResult);
		GetCharacterMovement()->Velocity = ComponentTransformWS.TransformVector(DeltaRootLocationRS) / DeltaTime;
		MovementData.LastUpdatedRootLocationRS = TargetRootLocationRS;
	}

	GetMovementFeatureAgent()->OrientationAimData.Direction = MovementDirectionWS;

	MovementData.MovementElapsedTime = FMath::Clamp(MovementData.MovementElapsedTime + DeltaTime * MovementData.CachedParams.AnimPlayRate, 0.f, MovementData.AnimPtr->GetSafePlayTimeRange().Y);
	
	bool bShouldBreakOut = false;
	if (FMath::IsNearlyEqual(MovementData.MovementElapsedTime, MovementData.AnimPtr->GetSafePlayTimeRange().Y, 0.001f)) { bShouldBreakOut = true; }
	else if (MovementData.SnappedParams.MovementType == EVSMantleVaultMovementType::Mantle)
	{
		if (MovementData.MovementElapsedTime >= MovementData.AnimPtr->GetMarkTime(AnimGroundPivotTimeMarkName))
		{
			if (HasMovementInput2D()) { bShouldBreakOut = true; }
			else if (MovementData.AnimSettingsPtr->MovementType & EVSMantleVaultMovementType::Vault) { bShouldBreakOut = true; }
		}
	}
	else if (MovementData.SnappedParams.MovementType == EVSMantleVaultMovementType::Vault)
	{
		if (MovementData.MovementElapsedTime >= MovementData.AnimPtr->GetMarkTime(AnimVaultOffPlatformTimeMarkName) && HasMovementInput2D())
		{
			const FVector& DeltaHeightWS = ComponentTransformWS.TransformVector((TargetRootLocationRS - MovementData.SnappedParams.GroundPivotPointRS).ProjectOnToNormal(UpVectorRS));
			if (DeltaHeightWS.Size() >= GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
				&& (TargetRootLocationWS - ComponentTransformWS.TransformPosition(MovementData.SnappedParams.VaultOffPlatformPointRS)).ProjectOnToNormal(MovementDirectionWS).Dot(MovementDirectionWS) > 0.f)
			{
				bShouldBreakOut = true;
			}
		}
	}
	
	if (bShouldBreakOut)
	{
		StopMantleVault();
	}
}

void UVSChrMovFeature_MantleVaultMovement::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	const FGameplayTagContainer& GameplayTags = GameplayTagController->GetGameplayTags();
	
	MovementData.bMatchesEntranceTagQuery = EntranceTagQuery.Matches(TagEvent, GameplayTags);
	
	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (!IsMantlingOrVaultingMode() && GetPrevMovementMode() == EVSMovementMode::MantlingOrVaulting && MovementData.SnappedParams.MovementType != EVSMantleVaultMovementType::None)
		{
			StopMantleVault();
		}
	}
	else if (IsMantlingOrVaultingMode() && AutoBreakTagQuery.Matches(TagEvent, GameplayTags))
	{
		StopMantleVault();
	}
}

bool UVSChrMovFeature_MantleVaultMovement::TryMantleVaultInternal(const TArray<FDataTableRowHandle>& SettingRows, const FVector& WallTraceDirection, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType)
{
	if (SupportedMovementType == EVSMantleVaultMovementType::None || (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty())) return false;
	
	const TArray<FDataTableRowHandle> SettingRowsToUse = SettingRows.IsEmpty() ? DefaultSettingRows : SettingRows;
	FVSMantleVaultSnappedParams SnappedParams = FVSMantleVaultSnappedParams();
	for (const FDataTableRowHandle& SettingsRowToUse : SettingRowsToUse)
	{
		if (CalcMantleVaultSnappedParams(SnappedParams, SettingsRowToUse, WallTraceDirection, SupportedMovementType))
		{
			MantleVaultBySnappedParams(SnappedParams);
			return true;
		}
	}

	return false;
}

void UVSChrMovFeature_MantleVaultMovement::MantleVaultBySnappedParams(const FVSMantleVaultSnappedParams& SnappedParams)
{
	if (SnappedParams.SettingsRow.IsNull()) return;
	
	MovementData.SnappedParams = SnappedParams;
	MovementData.SettingsPtr = MovementData.SnappedParams.SettingsRow.GetRow<FVSMantleVaultSettings>(nullptr);
	MovementData.AnimSettingsPtr = MovementData.SnappedParams.AnimSettingsRow.GetRow<FVSMantleVaultAnimSettings>(nullptr);
	MovementData.AnimPtr = MovementData.AnimSettingsPtr->AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);

	UPrimitiveComponent* Component = SnappedParams.Actor.IsValid() ? Cast<UPrimitiveComponent>(UVSActorLibrary::GetActorComponentByName(SnappedParams.Actor.Get(), SnappedParams.ComponentName)) : nullptr;
	if (!Component)
	{
		/** Retrace the component. */
		const FCollisionShape& SphereTraceShape = FCollisionShape::MakeSphere(1.f);
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(GetCharacter());
		FHitResult ComponentTraceHit;
		const FVector& StartFrontWallPointWS = SnappedParams.StartComponentTransform.TransformPosition(SnappedParams.FrontWallPointRS);
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, ComponentTraceHit, StartFrontWallPointWS, StartFrontWallPointWS - GetUpDirection() * 16.f, FQuat::Identity,
			SphereTraceShape, GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceParams);
		if (!ComponentTraceHit.bBlockingHit) return;
		MovementData.SnappedParams.Actor = ComponentTraceHit.GetActor();
		MovementData.SnappedParams.ComponentName = ComponentTraceHit.GetComponent()->GetFName();
		MovementData.CachedParams.Component = ComponentTraceHit.Component;
	}
	else
	{
		MovementData.CachedParams.Component = Component;
	}
	MovementData.CachedParams.AnimPlayRate = MovementData.AnimPtr->PlayRate;
	MovementData.CachedParams.ActionID = FMath::RandRange(0, INT16_MAX);
	
	const FVector& RootLocationWS = GetRootLocation();
	const FTransform& ComponentTransformWS = MovementData.CachedParams.Component->GetComponentTransform();
	const FVector& UpVectorRS = ComponentTransformWS.InverseTransformVectorNoScale(GetUpDirection());
	const FVector& UpVectorUnitRS = ComponentTransformWS.InverseTransformVector(GetUpDirection() * GetScale3D().Z);
	const FVector& MovementDirectionWS = ComponentTransformWS.TransformVectorNoScale(SnappedParams.MovementDirection2DRS);
	const FVector& MovementDirectionUnitRS = ComponentTransformWS.InverseTransformVector(MovementDirectionWS * GetScale3D().X);
	const FVector& StartRootLocationRS = UKismetMathLibrary::InverseTransformLocation(ComponentTransformWS, RootLocationWS);
	const float DistanceToWall2DRS = (SnappedParams.FrontWallPointRS - StartRootLocationRS).ProjectOnToNormal(SnappedParams.MovementDirection2DRS).Size() / MovementDirectionUnitRS.Size();
	const float PlatformHeightRS = (SnappedParams.GroundPivotPointRS - StartRootLocationRS).ProjectOnToNormal(UpVectorRS).Size();
	const bool bShouldScaleMovementToReachTarget = !MovementData.AnimPtr->HasTimeMark(AnimScaleMovementToReachTargetTimeMarkName) || MovementData.AnimPtr->GetMarkTime(AnimScaleMovementToReachTargetTimeMarkName) < MovementData.AnimPtr->GetMarkTime(AnimReachTargetTimeMarkName);

	MovementData.CachedParams.StartRootLocationRS = StartRootLocationRS;
	MovementData.CachedParams.ClientSideServerStartTime = UVSGameplayLibrary::GetServerTimeSeconds(this);
	MovementData.CachedParams.AnimStartMovementCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.AnimPtr->GetSafePlayTimeRange().X);
	MovementData.CachedParams.AnimStartMovementCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.AnimPtr->GetSafePlayTimeRange().X);
	if (bShouldScaleMovementToReachTarget && MovementData.AnimPtr->HasTimeMark(AnimScaleMovementToReachTargetTimeMarkName))
	{
		MovementData.CachedParams.AnimScaleMovementToReachTargetCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.AnimPtr->GetMarkTime(AnimScaleMovementToReachTargetTimeMarkName));
		MovementData.CachedParams.AnimScaleMovementToReachTargetCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.AnimPtr->GetMarkTime(AnimScaleMovementToReachTargetTimeMarkName));
	}
	MovementData.CachedParams.AnimReachTargetMovementValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.AnimPtr->GetMarkTime(AnimReachTargetTimeMarkName));
	MovementData.CachedParams.AnimReachTargetMovementValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.AnimPtr->GetMarkTime(AnimReachTargetTimeMarkName));
	MovementData.CachedParams.AnimGroundPivotMovementValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.AnimPtr->GetMarkTime(AnimGroundPivotTimeMarkName));
	MovementData.CachedParams.AnimGroundPivotMovementValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.AnimPtr->GetMarkTime(AnimGroundPivotTimeMarkName));
	
	if (SnappedParams.MovementType == EVSMantleVaultMovementType::Vault)
	{
		MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.AnimPtr->GetMarkTime(AnimVaultOffPlatformTimeMarkName));
		MovementData.CachedParams.AnimVaultOffPlatformMovementCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.AnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.AnimPtr->GetMarkTime(AnimVaultOffPlatformTimeMarkName));
	}
	
	if (bShouldScaleMovementToReachTarget && MovementData.AnimPtr->HasTimeMark(AnimScaleMovementToReachTargetTimeMarkName))
	{
		MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS = StartRootLocationRS;
		MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS += (MovementData.CachedParams.AnimScaleMovementToReachTargetCurveValues.X - MovementData.CachedParams.AnimStartMovementCurveValues.X) * MovementDirectionUnitRS;
		MovementData.CachedParams.ScaleMovementToReachTargetRootLocationRS += (MovementData.CachedParams.AnimScaleMovementToReachTargetCurveValues.Y - MovementData.CachedParams.AnimStartMovementCurveValues.Y) * UpVectorUnitRS;
	}

	// MovementData.CachedParams.ReachTargetRootLocationScaledRS = StartRootLocationRS;
	// MovementData.CachedParams.ReachTargetRootLocationScaledRS += (DistanceToWall2DRS - SnappedParams.AnimReachTargetDistanceToWall2D) * MovementDirectionUnitRS;
	// MovementData.CachedParams.ReachTargetRootLocationScaledRS += PlatformHeightRS * UpVectorRS - (MovementData.CachedParams.AnimGroundPivotMovementValues.Y - MovementData.CachedParams.AnimReachTargetMovementValues.Y) * UpVectorUnitRS;

	MovementData.CachedParams.ReachTargetRootLocationRS = StartRootLocationRS;
	MovementData.CachedParams.ReachTargetRootLocationRS += bShouldScaleMovementToReachTarget
		? (DistanceToWall2DRS - MovementData.SnappedParams.AnimReachTargetDistanceToWall2D) * MovementDirectionUnitRS
		: (MovementData.CachedParams.AnimReachTargetMovementValues.X - MovementData.CachedParams.AnimStartMovementCurveValues.X) * MovementDirectionUnitRS;
	MovementData.CachedParams.ReachTargetRootLocationRS += bShouldScaleMovementToReachTarget
		? PlatformHeightRS * UpVectorRS - (MovementData.CachedParams.AnimGroundPivotMovementValues.Y - MovementData.CachedParams.AnimReachTargetMovementValues.Y) * UpVectorUnitRS
		: (MovementData.CachedParams.AnimReachTargetMovementValues.Y - MovementData.CachedParams.AnimStartMovementCurveValues.Y) * UpVectorUnitRS;
	
	MovementData.MovementElapsedTime = MovementData.AnimPtr->GetSafePlayTimeRange().X;
	MovementData.LastUpdatedRootLocationRS = StartRootLocationRS;
	
	if (!IsMantlingOrVaultingMode())
	{
		SetMovementMode(EVSMovementMode::MantlingOrVaulting, false);
	}
	GetCharacterMovement()->StopMovementImmediately();
	const float TargetCapsuleHalfHeightUSC = MovementData.SettingsPtr->CapsuleHalfHeight > 0.f ? MovementData.SettingsPtr->CapsuleHalfHeight : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MovementData.CapsuleHalfHeightOffsetUSCZ = TargetCapsuleHalfHeightUSC - GetCharacter()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(TargetCapsuleHalfHeightUSC);
}

bool UVSChrMovFeature_MantleVaultMovement::CalcMantleVaultSnappedParams(FVSMantleVaultSnappedParams& OutSnappedParams, const FDataTableRowHandle& SettingsRow, const FVector& WallTraceDirection, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType) const
{
	FVSMantleVaultSettings* SettingsPtr = SettingsRow.GetRow<FVSMantleVaultSettings>(nullptr);
	if (!SettingsPtr || !SettingsPtr->IsValid()) return false;

	if (SettingsPtr->Limits.bRequireMovementInput2D && !HasMovementInput2D()) return false;
	if (!SettingsPtr->Limits.MovementTagQuery.IsEmpty() && !SettingsPtr->Limits.MovementTagQuery.Matches(GetGameplayTagController()->GetGameplayTags())) return false;

	const FVector& CharacterScaleWS = GetCharacter()->GetActorScale3D();

	TArray<FDataTableRowHandle> AvailableAnimRows;
	for (const auto& AnimRow : SettingsPtr->AnimRows)
	{
		FVSMantleVaultAnimSettings* Anim = AnimRow.GetRow<FVSMantleVaultAnimSettings>(nullptr);
		if (!Anim || !Anim->IsValid(AnimScaleMovementToReachTargetTimeMarkName, AnimReachTargetTimeMarkName, AnimGroundPivotTimeMarkName, AnimVaultOffPlatformTimeMarkName)) continue;
		AvailableAnimRows.Add(AnimRow);
	}
	if (AvailableAnimRows.IsEmpty()) return false;
	Algo::RandomShuffle(AvailableAnimRows);

	/** This is absolute value. */
	const FVector& VerticalCollisionOffset = 2.f * GetUpDirection();

	/** SC means scaled. */
	const float CapsuleHalfHeightSC = SettingsPtr->CapsuleHalfHeight > 0.f ? (SettingsPtr->CapsuleHalfHeight * CharacterScaleWS.Z) : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float CapsuleRadiusSC = GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector& RootLocationWS = GetRootLocation();
	FVector2D PlatformHeightRangeSC = SettingsPtr->Limits.PlatformHeightRange * CharacterScaleWS.Z;

	const FCollisionResponseContainer& CollisionChannels = GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels();
	const FCollisionShape& HalfHeightTraceShape = FCollisionShape::MakeCapsule(CapsuleRadiusSC, CapsuleHalfHeightSC);
	const FCollisionShape& RadiusSphereTraceShape = FCollisionShape::MakeSphere(CapsuleRadiusSC);
	FCollisionQueryParams TraceQueryParams;
	TraceQueryParams.AddIgnoredActor(GetCharacter());
	
	/** Check vertical space block and update the platform height range max. */
	const FVector& VerticalSpaceTraceStart = RootLocationWS + GetCharacter()->GetActorUpVector() * CapsuleHalfHeightSC;
	const FVector& VerticalSpaceTraceEnd = RootLocationWS + GetCharacter()->GetActorUpVector() * (PlatformHeightRangeSC.Y + CapsuleHalfHeightSC);
	FHitResult VerticalSpaceHitResult;
	const bool bHasVerticalBlock = UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, VerticalSpaceHitResult, VerticalSpaceTraceStart, VerticalSpaceTraceEnd, FQuat::Identity, HalfHeightTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (bDrawDebugShapes) { DrawDebugCapsuleTraceSingle(GetWorld(), VerticalSpaceTraceStart, VerticalSpaceTraceEnd, CapsuleRadiusSC, CapsuleHalfHeightSC, EDrawDebugTrace::ForOneFrame, VerticalSpaceHitResult.bBlockingHit, VerticalSpaceHitResult, FColor::Red, FColor::Green, 3.f); }
#endif
	if (bHasVerticalBlock)
	{
		if (!VerticalSpaceHitResult.IsValidBlockingHit()) return false;
		PlatformHeightRangeSC.Y = PlatformHeightRangeSC.Y * ((VerticalSpaceHitResult.Location - VerticalSpaceTraceStart).Size() / (VerticalSpaceTraceEnd - VerticalSpaceTraceStart).Size());
		if (PlatformHeightRangeSC.Y < PlatformHeightRangeSC.X) return false;
	}
	
	const FQuat& UpVectorToWorldRotation = FQuat::FindBetweenNormals(FVector::UpVector, GetUpDirection());
	const FVector& InputDS = UKismetMathLibrary::Quat_RotateVector(UpVectorToWorldRotation, GetMovementInput2D());
	const FVector& VelocityDS = UKismetMathLibrary::Quat_RotateVector(UpVectorToWorldRotation, GetVelocityWallAdjusted2D());
	FVector FrontWallTraceDirection2D = WallTraceDirection;
	if (WallTraceDirection.IsNearlyZero())
	{
		FrontWallTraceDirection2D = GetCharacter()->GetActorForwardVector();
		if (InputDS.Size2D() > 1.f)
		{
			FrontWallTraceDirection2D = GetMovementInput2D().GetSafeNormal();
		}
		else if (VelocityDS.Size2D() > 1.f)
		{
			FrontWallTraceDirection2D = GetVelocityWallAdjusted2D().GetSafeNormal();
		}
	}

	/** Do the trace process. This is splitted into multiple heights. */
	for (float i = PlatformHeightRangeSC.X; i < PlatformHeightRangeSC.Y + CapsuleRadiusSC; i += CapsuleRadiusSC)
	{
		/** Trace for wall towards movement. */
		FVector FrontWallNormal2DWS = FVector::ZeroVector;
		FVector FrontWallPointWS = FVector::ZeroVector;
		const FVector FrontWallTraceStart = RootLocationWS + FMath::Max(i - CapsuleRadiusSC, CapsuleRadiusSC) * GetCharacter()->GetActorUpVector();
		const FVector FrontWallTraceEnd = FrontWallTraceStart + SettingsPtr->TraceForwardDistance * FrontWallTraceDirection2D * CharacterScaleWS.X;
		FHitResult FrontWallTraceHitResult;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, FrontWallTraceHitResult, FrontWallTraceStart, FrontWallTraceEnd, FQuat::Identity, RadiusSphereTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
		if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), FrontWallTraceStart, FrontWallTraceEnd, CapsuleRadiusSC, EDrawDebugTrace::ForOneFrame, FrontWallTraceHitResult.bBlockingHit, FrontWallTraceHitResult, FColor::Blue, FColor::Yellow, 3.f); }
#endif
		/** No front wall. */
		if (!FrontWallTraceHitResult.IsValidBlockingHit()) continue;
		
		FrontWallNormal2DWS = UKismetMathLibrary::ProjectVectorOnToPlane(FrontWallTraceHitResult.ImpactNormal, GetUpDirection()).GetSafeNormal();
		FrontWallPointWS = FrontWallTraceHitResult.ImpactPoint;
		const FVector& MovementDirection2DWS = -FrontWallNormal2DWS;


		/** Check the limits for front wall. */
		bool bMeetTowardsMovementRequirementsByWall = true;
		const float DistanceToWallSC = ((FrontWallPointWS - RootLocationWS).ProjectOnToNormal(MovementDirection2DWS) / CharacterScaleWS.X).Size();
		if (!UKismetMathLibrary::InRange_FloatFloat(DistanceToWallSC, SettingsPtr->Limits.DistanceToWallRange2D.X, SettingsPtr->Limits.DistanceToWallRange2D.Y)) { bMeetTowardsMovementRequirementsByWall = false; }
		if (!UKismetMathLibrary::InRange_FloatFloat(GetVelocity().ProjectOnToNormal(MovementDirection2DWS).Size() * CharacterScaleWS.X, SettingsPtr->Limits.SpeedTowardsMovementRange2D.X, SettingsPtr->Limits.SpeedTowardsMovementRange2D.Y)) { bMeetTowardsMovementRequirementsByWall = false; }
		else if (GetCharacter()->GetActorForwardVector().Dot(-FrontWallNormal2DWS) < UKismetMathLibrary::DegCos(SettingsPtr->Limits.FacingMovementMaxAngle)) bMeetTowardsMovementRequirementsByWall = false;
		else if (IsMoving2D() && !IsMovingAgainstWall2D() && (GetVelocityWallAdjusted2D().GetSafeNormal()).Dot(-FrontWallNormal2DWS) < UKismetMathLibrary::DegCos(SettingsPtr->Limits.Velocity2DTowardsMovementMaxAngle)) { bMeetTowardsMovementRequirementsByWall = false; }
		else if (IsMoving2D() && IsMovingAgainstWall2D() && (GetMovementInput2D().GetSafeNormal()).Dot(-FrontWallNormal2DWS) < UKismetMathLibrary::DegCos(SettingsPtr->Limits.Velocity2DTowardsMovementMaxAngle)) { bMeetTowardsMovementRequirementsByWall = false; }
		else if (HasMovementInput2D() && (GetMovementInput2D().GetSafeNormal()).Dot(-FrontWallNormal2DWS) < UKismetMathLibrary::DegCos(SettingsPtr->Limits.Input2DTowardsMovementMaxAngle)) { bMeetTowardsMovementRequirementsByWall = false; }
		if (!bMeetTowardsMovementRequirementsByWall) continue;

		/** Calculate for anims. */
		for (const FDataTableRowHandle& AvailableAnimRow : AvailableAnimRows)
		{
			FVSMantleVaultAnimSettings* AnimSettings = AvailableAnimRow.GetRow<FVSMantleVaultAnimSettings>(nullptr);
			if (!(AnimSettings->MovementType & SupportedMovementType)) continue;
			FVSAnimSequenceReference* Anim = AnimSettings->AnimRow.GetRow<FVSAnimSequenceReference>(nullptr);
			if (!Anim || !Anim->IsValid()) continue;

			const bool bSupportVault = (SupportedMovementType & EVSMantleVaultMovementType::Vault) && (AnimSettings->MovementType & EVSMantleVaultMovementType::Vault);
			if (AnimSettings->MovementType == EVSMantleVaultMovementType::Vault && !bSupportVault) continue;

			/** Extract data from settings and animation. */

			float AnimStartMovementX;
			AnimStartMovementX = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence,UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), 0.0);
			float AnimReachTargetWallMovementX;
			AnimReachTargetWallMovementX = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), Anim->GetMarkTime(AnimReachTargetTimeMarkName));
			float AnimGroundPivotMovementX;
			AnimGroundPivotMovementX = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), Anim->GetMarkTime(AnimGroundPivotTimeMarkName));
			float AnimVaultOffPlatformMovementX;
			AnimVaultOffPlatformMovementX = UVSAnimationLibrary::GetAnimationCurveValueAtTime(Anim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), Anim->GetMarkTime(AnimVaultOffPlatformTimeMarkName));
			const float DistanceToWall2DWS = (FrontWallPointWS - RootLocationWS).ProjectOnToNormal(MovementDirection2DWS).Size();
			const float AnimReachTargetDistanceToWall2DNoScaleWS = (AnimReachTargetWallMovementX - AnimStartMovementX) * CharacterScaleWS.X;
			// FVector ReachTargetWallRootLocationNotScaledXWS = RootLocationWS;
			// ReachTargetWallRootLocationNotScaledXWS += (AnimReachTargetWallMovementX - AnimStartMovementX) * MovementDirection2DWS * CharacterScaleWS.X;
			FVector ReachTargetWallRootLocationScaledFarthestXWS = RootLocationWS;
			ReachTargetWallRootLocationScaledFarthestXWS += (DistanceToWall2DWS - AnimSettings->ReachTargetDistanceToWallRange.X * CharacterScaleWS.X) * MovementDirection2DWS;
			FVector GroundPivotLocationFarthestXWS = ReachTargetWallRootLocationScaledFarthestXWS;
			GroundPivotLocationFarthestXWS += (AnimGroundPivotMovementX - AnimReachTargetWallMovementX) * MovementDirection2DWS * CharacterScaleWS.X;
			FVector VaultOffPlatformRootLocationFarthestXWS = ReachTargetWallRootLocationScaledFarthestXWS;
			VaultOffPlatformRootLocationFarthestXWS += (AnimVaultOffPlatformMovementX - AnimReachTargetWallMovementX) * MovementDirection2DWS * CharacterScaleWS.X;
			
			/** Check horizontal space. Higher one radius. */
			const FVector& HorizontalSpaceTraceLocationFarthestWS = bSupportVault ? VaultOffPlatformRootLocationFarthestXWS : GroundPivotLocationFarthestXWS;
			const FVector& HorizontalSpaceTraceOffset = UKismetMathLibrary::Vector_ProjectOnToNormal(HorizontalSpaceTraceLocationFarthestWS - GetCharacter()->GetActorLocation(), MovementDirection2DWS);
			const FVector& HorizontalSpaceStart = RootLocationWS + (i + CapsuleRadiusSC) * GetCharacter()->GetActorUpVector();
			const FVector& HorizontalSpaceEnd = HorizontalSpaceStart + HorizontalSpaceTraceOffset;
			FHitResult HorizontalSpaceHitResult;
			UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, HorizontalSpaceHitResult, HorizontalSpaceStart, HorizontalSpaceEnd, FQuat::Identity, RadiusSphereTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
			if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), HorizontalSpaceStart, HorizontalSpaceEnd, CapsuleRadiusSC, EDrawDebugTrace::ForOneFrame, HorizontalSpaceHitResult.bBlockingHit, HorizontalSpaceHitResult, FColor::Cyan, FColor::Magenta, 3.f); }
#endif
			const float DeltaReachTargetSize = (HorizontalSpaceTraceLocationFarthestWS - RootLocationWS).ProjectOnToNormal(MovementDirection2DWS).Size() - ((HorizontalSpaceHitResult.bBlockingHit ? HorizontalSpaceHitResult.Location : HorizontalSpaceHitResult.TraceEnd) - RootLocationWS).ProjectOnToNormal(MovementDirection2DWS).Size();
			const float AnimReachTargetWallDistanceToWall2DMinByHorizontalSpace = AnimSettings->ReachTargetDistanceToWallRange.X + DeltaReachTargetSize / CharacterScaleWS.X;
			/** No enough horizontal space. */
			if (!UKismetMathLibrary::InRange_FloatFloat(AnimReachTargetWallDistanceToWall2DMinByHorizontalSpace, AnimSettings->ReachTargetDistanceToWallRange.X - 0.01f, AnimSettings->ReachTargetDistanceToWallRange.Y + 0.01f)) continue;

			/** Check for vertical block at the farthest end. */
			const FVector& FarthestVerticalBlockTraceStart = (HorizontalSpaceHitResult.bBlockingHit ? HorizontalSpaceHitResult.Location : HorizontalSpaceHitResult.TraceEnd) + CapsuleRadiusSC * MovementDirection2DWS;
			const FVector& FarthestVerticalBlockTraceEnd = FarthestVerticalBlockTraceStart - 2.f * CapsuleRadiusSC * GetUpDirection();
			FHitResult FarthestVerticalBlockHitResult;
			UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, FarthestVerticalBlockHitResult, FarthestVerticalBlockTraceStart, FarthestVerticalBlockTraceEnd, FQuat::Identity, RadiusSphereTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
			if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), FarthestVerticalBlockTraceStart, FarthestVerticalBlockTraceEnd, CapsuleRadiusSC, EDrawDebugTrace::ForOneFrame, FarthestVerticalBlockHitResult.bBlockingHit, FarthestVerticalBlockHitResult, FColor::Orange, FColor::Purple, 3.f); }
#endif

			/** Check for possible wall at the other side. */
			const FVector& OtherSideWallStart = (FarthestVerticalBlockHitResult.bBlockingHit ? FarthestVerticalBlockHitResult.Location : FarthestVerticalBlockHitResult.TraceEnd) + 0.f;
			const FVector& OtherSideWallEnd = OtherSideWallStart - HorizontalSpaceTraceOffset;
			FHitResult OtherSideWallHitResult;
			UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, OtherSideWallHitResult, OtherSideWallStart, OtherSideWallEnd, FQuat::Identity, RadiusSphereTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
			if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), OtherSideWallStart, OtherSideWallEnd, CapsuleRadiusSC, EDrawDebugTrace::ForOneFrame, OtherSideWallHitResult.bBlockingHit, OtherSideWallHitResult, FColor::Orange, FColor::Purple, 3.f); }
#endif

			/** Check wall thickness. */
			const float DeltaGroundPivotSize = ((OtherSideWallHitResult.bBlockingHit ? OtherSideWallHitResult.ImpactPoint : HorizontalSpaceTraceLocationFarthestWS) - RootLocationWS).ProjectOnToNormal(MovementDirection2DWS).Size() - (HorizontalSpaceTraceLocationFarthestWS - RootLocationWS).ProjectOnToNormal(MovementDirection2DWS).Size();
			const float AnimReachTargetWallDistanceToWall2DMinByWallThickness = AnimSettings->ReachTargetDistanceToWallRange.X + (DeltaGroundPivotSize >= 0.f ? 0.f : -DeltaGroundPivotSize / CharacterScaleWS.X);
			/** The wall is too thin or the platform is too short. */
			if (!UKismetMathLibrary::InRange_FloatFloat(AnimReachTargetWallDistanceToWall2DMinByWallThickness, AnimSettings->ReachTargetDistanceToWallRange.X - 0.01f, AnimSettings->ReachTargetDistanceToWallRange.Y + 0.01f)) continue;
			
			const float ReachTargetWallDistanceToWall2DMin = FMath::Max(AnimReachTargetWallDistanceToWall2DMinByHorizontalSpace, AnimReachTargetWallDistanceToWall2DMinByWallThickness);
			const float ReachTargetWallDistanceToWall2D = FMath::Clamp(AnimReachTargetDistanceToWall2DNoScaleWS, ReachTargetWallDistanceToWall2DMin, AnimSettings->ReachTargetDistanceToWallRange.Y);
			FVector CurrentReachTargetWallRootLocationXWS = RootLocationWS + (DistanceToWall2DWS - ReachTargetWallDistanceToWall2D * CharacterScaleWS.X) * MovementDirection2DWS;
			FVector CurrentGroundPivotTimeRootLocationXWS = CurrentReachTargetWallRootLocationXWS;
			CurrentGroundPivotTimeRootLocationXWS += (AnimGroundPivotMovementX - AnimReachTargetWallMovementX) * MovementDirection2DWS * CharacterScaleWS.X;
			
			/** Check for the current ground pivot point. */
			const FVector& GroundPivotVerticalTraceOffset = UKismetMathLibrary::Vector_ProjectOnToNormal(CurrentGroundPivotTimeRootLocationXWS - GetCharacter()->GetActorLocation(), MovementDirection2DWS);
			const FVector& GroundPivotVerticalTraceStart = RootLocationWS + GroundPivotVerticalTraceOffset + (i + CapsuleRadiusSC + 0.01f) * GetUpDirection() - VerticalCollisionOffset;
			const FVector& GroundPivotVerticalTraceEnd = GroundPivotVerticalTraceStart - CapsuleRadiusSC * GetUpDirection();
			FHitResult GroundPivotVerticalTraceHitResult;
			UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, GroundPivotVerticalTraceHitResult, GroundPivotVerticalTraceStart, GroundPivotVerticalTraceEnd, FQuat::Identity, RadiusSphereTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
			if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), GroundPivotVerticalTraceStart, GroundPivotVerticalTraceEnd, CapsuleRadiusSC, EDrawDebugTrace::ForOneFrame, GroundPivotVerticalTraceHitResult.bBlockingHit, GroundPivotVerticalTraceHitResult, FColor::Red, FColor::Green, 3.f); }
#endif
			/** No ground point on the platform. */
			if (!GroundPivotVerticalTraceHitResult.IsValidBlockingHit()) continue;


			/** Check terrain object type here. */
			bool bTerrainObjectTypeValid = true;
			if (!SettingsPtr->Limits.TerrainComponentQuery.Matches(FrontWallTraceHitResult.GetComponent())) { bTerrainObjectTypeValid = false; }
			if (!SettingsPtr->Limits.TerrainComponentQuery.Matches(GroundPivotVerticalTraceHitResult.GetComponent())) { bTerrainObjectTypeValid = false; }
			if (!SettingsPtr->Limits.TerrainComponentQuery.Matches(GroundPivotVerticalTraceHitResult.GetComponent())) { bTerrainObjectTypeValid = false; }
			if (!bTerrainObjectTypeValid) continue;
			
			const float PlatformHeightSC = (GroundPivotVerticalTraceHitResult.ImpactPoint - RootLocationWS + VerticalCollisionOffset).ProjectOnToNormal(GetUpDirection()).Size() / CharacterScaleWS.Z;
			/** Invalid platform height. */
			if (!UKismetMathLibrary::InRange_FloatFloat(PlatformHeightSC, SettingsPtr->Limits.PlatformHeightRange.X, SettingsPtr->Limits.PlatformHeightRange.Y)) continue;

			/** TODO Refine vertical space limitation for vaulting. By defaylt this is set to one radius. */
			bool bSupportVaultInternal = bSupportVault;
			if (FarthestVerticalBlockHitResult.bBlockingHit)
			{
				if ((FarthestVerticalBlockHitResult.ImpactPoint - GroundPivotVerticalTraceHitResult.ImpactPoint).ProjectOnToNormal(GetUpDirection()).Size() < CapsuleRadiusSC)
				{
					bSupportVaultInternal = false;
				}
			}
			if (AnimSettings->MovementType == EVSMantleVaultMovementType::Vault && !bSupportVaultInternal) continue;
			
			/** Check for character room on the ground pivot. */
			const FVector& CharacterRoomTraceStart = GroundPivotVerticalTraceHitResult.Location + (CapsuleHalfHeightSC - CapsuleRadiusSC + 0.01f) * GetUpDirection() + VerticalCollisionOffset;
			const FVector& CharacterRoomTraceEnd = CharacterRoomTraceStart - 0.01f * GetUpDirection();
			FHitResult CharacterRoomTraceHitResult;
			UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, CharacterRoomTraceHitResult, CharacterRoomTraceStart, CharacterRoomTraceEnd, FQuat::Identity, HalfHeightTraceShape, CollisionChannels, TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
			if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), CharacterRoomTraceStart, CharacterRoomTraceEnd, CapsuleRadiusSC, EDrawDebugTrace::ForOneFrame, CharacterRoomTraceHitResult.bBlockingHit, CharacterRoomTraceHitResult, FColor::Blue, FColor::Yellow, 3.f); }
#endif
			/** No enough room for character to get to ground pivot. */
			if (CharacterRoomTraceHitResult.bBlockingHit) continue;

			/** This is a valid mantle / vault animation. */
			const FTransform& ComponentTransformWS = GroundPivotVerticalTraceHitResult.Component->GetComponentTransform();

			
			OutSnappedParams.SettingsRow = SettingsRow;
			OutSnappedParams.AnimSettingsRow = AvailableAnimRow;
			OutSnappedParams.Actor = GroundPivotVerticalTraceHitResult.GetActor();
			OutSnappedParams.ComponentName = GroundPivotVerticalTraceHitResult.GetComponent()->GetFName();
			OutSnappedParams.StartComponentTransform = ComponentTransformWS;
			OutSnappedParams.MovementDirection2DRS = ComponentTransformWS.InverseTransformVectorNoScale(MovementDirection2DWS);
			OutSnappedParams.FrontWallPointRS = ComponentTransformWS.InverseTransformPosition(FrontWallPointWS);
			OutSnappedParams.GroundPivotPointRS = ComponentTransformWS.InverseTransformPosition(GroundPivotVerticalTraceHitResult.ImpactPoint + VerticalCollisionOffset);
			OutSnappedParams.MovementType = (bSupportVault && bSupportVaultInternal) ? EVSMantleVaultMovementType::Vault : EVSMantleVaultMovementType::Mantle;
			OutSnappedParams.ServerSideServerStartTime = UVSGameplayLibrary::GetServerTimeSeconds(this);

			if (OutSnappedParams.MovementType == EVSMantleVaultMovementType::Vault)
			{
				const FVector& VaultOffPlatformLocationWS = UVSMathLibrary::VectorApplyAxes((OtherSideWallHitResult.ImpactPoint + MovementDirection2DWS * CapsuleRadiusSC), GroundPivotVerticalTraceHitResult.ImpactPoint + VerticalCollisionOffset, EVSVectorAxes::Z, GetCharacter()->GetActorRotation());
				OutSnappedParams.VaultOffPlatformPointRS = ComponentTransformWS.InverseTransformPosition(VaultOffPlatformLocationWS);
			}
			
			return true;
		}
	}
	
	return false;
}


void UVSChrMovFeature_MantleVaultMovement::TryMantleVault_Server_Implementation(const TArray<FDataTableRowHandle>& SettingRows, const FVector& WallTraceDirection, EVSMantleVaultMovementType::Type SupportedMovementType, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		const bool bSuccessful = TryMantleVaultInternal(SettingRows, WallTraceDirection, SupportedMovementType);
		if (bSuccessful && NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			MantleVault_Multicast(MovementData.SnappedParams, NetExecPolicy);
		}
	}
}

void UVSChrMovFeature_MantleVaultMovement::MantleVault_Server_Implementation(const FVSMantleVaultSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		MantleVaultBySnappedParams(SnappedParams);
		if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
		{
			MantleVault_Multicast(MovementData.SnappedParams, NetExecPolicy);
		}
	}
}

void UVSChrMovFeature_MantleVaultMovement::MantleVault_Multicast_Implementation(const FVSMantleVaultSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;
	if (SnappedParams.SettingsRow.IsNull() || SnappedParams.AnimSettingsRow.IsNull()) { bShouldExecute = false; }
	else if (SnappedParams.MovementType == EVSMantleVaultMovementType::None) { bShouldExecute = false; }

	/** Authority already handled. */
	if (GetCharacter()->HasAuthority()) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		MantleVaultBySnappedParams(SnappedParams);
	}
}