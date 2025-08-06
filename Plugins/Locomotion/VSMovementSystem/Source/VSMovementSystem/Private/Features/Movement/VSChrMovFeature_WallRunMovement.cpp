// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_WallRunMovement.h"
#include "KismetTraceUtils.h"
#include "VSChrMovCapsuleComponent.h"
#include "VSMovementSystemSettings.h"
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
#include "Types/Animation/VSAnimSequenceReference.h"

UVSChrMovFeature_WallRunMovement::UVSChrMovFeature_WallRunMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovFeature_WallRunMovement::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (!IsWallRunMode() && GetPrevMovementMode() == EVSMovementMode::WallRunning && !MovementData.SnappedParams.SettingsRow.IsNull())
		{
			StopWallRun(false, FVSNetMethodExecutionPolicies::LocalExecution);
		}
	}
}

void UVSChrMovFeature_WallRunMovement::EndPlay_Implementation()
{
	if (IsWallRunMode())
	{
		StopWallRun(false, FVSNetMethodExecutionPolicies::LocalExecution);
	}
	
	Super::EndPlay_Implementation();
}

bool UVSChrMovFeature_WallRunMovement::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsWallRunMode() && MovementData.CachedParams.Component.IsValid();
}

void UVSChrMovFeature_WallRunMovement::UpdateMovement_Implementation(float DeltaTime)
{
	const FVector& RootLocationWS = GetRootLocation();
	FTransform ComponentTransformWS = MovementData.CachedParams.Component->GetComponentTransform();
	FVector UpVectorRS = UKismetMathLibrary::InverseTransformDirection(ComponentTransformWS, GetUpDirection());
	
	FCollisionQueryParams TraceQueryParams;
	TraceQueryParams.AddIgnoredActor(GetCharacter());
	
	/** Trace side wall root to adjust the movement direction. */
	bool bShouldTraceForSideWall = true;
	if (MovementData.WallRunState == EVSWallRunState::Starting)
	{
		const float StartReachTargetWallTime = MovementData.StartAnimPtr->HasTimeMark(AnimStartReachWallMarkName) ? MovementData.StartAnimPtr->GetMarkTime(AnimStartReachWallMarkName) : MovementData.StartAnimPtr->GetSafePlayTimeRange().Y;
		if (MovementData.StartMovementElapsedTime <= StartReachTargetWallTime)
		{
			bShouldTraceForSideWall = false;
		}
	}
	else if (MovementData.WallRunState == EVSWallRunState::Ending)
	{
		const float EndLeaveWallWallTime = MovementData.EndAnimPtr->HasTimeMark(AnimStartReachWallMarkName) ? MovementData.EndAnimPtr->GetMarkTime(AnimEndLeaveWallMarkName) : MovementData.EndAnimPtr->GetSafePlayTimeRange().Y;
		if (MovementData.EndMovementElapsedTime >= EndLeaveWallWallTime)
		{
			bShouldTraceForSideWall = false;
		}
	}
	FHitResult TraceSideWallFootHitResult;
	FHitResult TraceSideWallHandHitResult;
	/** Foot. */
	if (bShouldTraceForSideWall)
	{
		const float CapsuleRadiusSC = GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector& TraceSideWallStart = GetRootLocation() + FMath::Min(CapsuleRadiusSC, MovementData.SettingsPtr->RootFootHeightOffset) * GetUpDirection();
		const FVector& TraceSideWallEnd = TraceSideWallStart + ComponentTransformWS.TransformVectorNoScale(-MovementData.LastTracedWallNormalRS) * FMath::Max(MovementData.SettingsPtr->CycleDistanceToWall, MovementData.SettingsPtr->WallTraceOffset.Y) * GetScale3D().Y * 1.05f;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, TraceSideWallFootHitResult, TraceSideWallStart, TraceSideWallEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA
		if (bDrawDebugShapes) { DrawDebugLineTraceSingle(GetWorld(), TraceSideWallStart, TraceSideWallEnd, EDrawDebugTrace::ForOneFrame, TraceSideWallFootHitResult.bBlockingHit, TraceSideWallFootHitResult, FColor::Cyan, FColor::Magenta, 3.f); }
#endif
		if (TraceSideWallFootHitResult.IsValidBlockingHit())
		{
			/** Consider component change. */
			if (TraceSideWallFootHitResult.GetComponent() != MovementData.CachedParams.Component)
			{
				MovementData.CachedParams.Component = TraceSideWallFootHitResult.GetComponent();
				ComponentTransformWS = MovementData.CachedParams.Component->GetComponentTransform();
				UpVectorRS = UKismetMathLibrary::InverseTransformDirection(ComponentTransformWS, GetUpDirection());
			}
			
			MovementData.LastTracedWallNormalRS = ComponentTransformWS.InverseTransformVectorNoScale(TraceSideWallFootHitResult.ImpactNormal);
			MovementData.LastTracedWallPointRS = ComponentTransformWS.InverseTransformVectorNoScale(TraceSideWallFootHitResult.ImpactPoint);
		}
	}
	/** Hand. */
	if (bShouldTraceForSideWall)
	{
		const FVector& TraceSideWallStart = GetRootLocation() + MovementData.SettingsPtr->RootHandHeightOffset * GetUpDirection();
		const FVector& TraceSideWallEnd = TraceSideWallStart + ComponentTransformWS.TransformVectorNoScale(-MovementData.LastTracedWallNormalRS) * FMath::Max(MovementData.SettingsPtr->CycleDistanceToWall, MovementData.SettingsPtr->WallTraceOffset.Y) * GetScale3D().Y * 1.05f;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, TraceSideWallHandHitResult, TraceSideWallStart, TraceSideWallEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA
		if (bDrawDebugShapes) { DrawDebugLineTraceSingle(GetWorld(), TraceSideWallStart, TraceSideWallEnd, EDrawDebugTrace::ForDuration, TraceSideWallHandHitResult.bBlockingHit, TraceSideWallHandHitResult, FColor::Orange, FColor::Purple, 3.f); }
#endif
	}
	
	FVector MovementDirectionWS = GetUpDirection().Cross(MovementData.LastTracedWallNormalRS).GetSafeNormal() * (!MovementData.SnappedParams.bLeftOrRight ? -1.f : 1.f);

	/** Update the movement for different states. */
	FHitResult UpdateMovementHitResult;
	if (MovementData.WallRunState == EVSWallRunState::Starting)
	{
		const float ForwardMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.StartAnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.StartMovementElapsedTime);
		const float SideMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.StartAnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), MovementData.StartMovementElapsedTime);
		const float UpMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.StartAnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.StartMovementElapsedTime);

		const float StartReachTargetWallTime = MovementData.StartAnimPtr->HasTimeMark(AnimStartReachWallMarkName) ? MovementData.StartAnimPtr->GetMarkTime(AnimStartReachWallMarkName) : MovementData.StartAnimPtr->GetSafePlayTimeRange().Y;

		FVector TargetRootLocationWS = GetRootLocation();
		if (MovementData.StartMovementElapsedTime <= StartReachTargetWallTime)
		{
			FVector TargetRootLocationRS = MovementData.CachedParams.StartRootLocationRS;
			const FVector& DeltaToReachTargetWallRS = MovementData.CachedParams.StartReachWallRootLocationRS - MovementData.CachedParams.StartRootLocationRS;
			TargetRootLocationRS += (ForwardMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.X) / (MovementData.CachedParams.AnimStartReachWallMovementCurveValues.X - MovementData.CachedParams.AnimStartMovementCurveValues.X) * UKismetMathLibrary::Vector_ProjectOnToNormal(DeltaToReachTargetWallRS, MovementData.SnappedParams.StartMovementDirection2DRS);
			TargetRootLocationRS += (SideMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.Y) / (MovementData.CachedParams.AnimStartReachWallMovementCurveValues.Y - MovementData.CachedParams.AnimStartMovementCurveValues.Y) * UKismetMathLibrary::Vector_ProjectOnToNormal(DeltaToReachTargetWallRS, -MovementData.SnappedParams.StartWallNormal2DRS);
			TargetRootLocationRS += (UpMovementCurveValue - MovementData.CachedParams.AnimStartMovementCurveValues.Z) / (MovementData.CachedParams.AnimStartReachWallMovementCurveValues.Z - MovementData.CachedParams.AnimStartMovementCurveValues.Z) * UKismetMathLibrary::Vector_ProjectOnToNormal(DeltaToReachTargetWallRS, UpVectorRS);
			TargetRootLocationWS = ComponentTransformWS.TransformPosition(TargetRootLocationRS);
		}
		else
		{
			TargetRootLocationWS += (ForwardMovementCurveValue - MovementData.LastUpdatedStartMovementCurveValues.X) * MovementDirectionWS * GetCharacter()->GetActorScale3D().X;
			TargetRootLocationWS += (SideMovementCurveValue - MovementData.LastUpdatedStartMovementCurveValues.Y) * ComponentTransformWS.TransformVectorNoScale(-MovementData.LastTracedWallNormalRS) * GetCharacter()->GetActorScale3D().Y;
			TargetRootLocationWS += (UpMovementCurveValue - MovementData.LastUpdatedStartMovementCurveValues.Z) * GetUpDirection() * GetCharacter()->GetActorScale3D().Z;
		}
		
		const FVector& RootLocationDeltaWS = TargetRootLocationWS - RootLocationWS;
		GetCharacterMovement()->SafeMoveUpdatedComponent(TargetRootLocationWS - RootLocationWS, GetCharacter()->GetActorQuat(), false, UpdateMovementHitResult);
		GetCharacterMovement()->Velocity = RootLocationDeltaWS / DeltaTime;

		MovementData.LastUpdatedRootLocationRS = ComponentTransformWS.InverseTransformPosition(TargetRootLocationWS);
		MovementData.LastUpdatedStartMovementCurveValues = FVector(ForwardMovementCurveValue, SideMovementCurveValue, UpMovementCurveValue);
		
		MovementData.StartMovementElapsedTime = FMath::Clamp(MovementData.StartMovementElapsedTime + DeltaTime * MovementData.StartAnimPtr->PlayRate, 0.f, MovementData.StartAnimPtr->GetSafePlayTimeRange().Y);
		if (MovementData.StartMovementElapsedTime >= MovementData.StartAnimPtr->GetSafePlayTimeRange().Y)
		{
			SetWallRunState(EVSWallRunState::Cycling);
			GetCharacterMovement()->Velocity = UVSMathLibrary::VectorApplyAxes(GetCharacterMovement()->Velocity, FVector::ZeroVector, EVSVectorAxes::Z, GetCharacter()->GetActorRotation());
		}
	}
	else if (MovementData.WallRunState == EVSWallRunState::Cycling)
	{
		// float SpeedSizeDelta = GetVelocity2D().Size() - MovementData.SettingsPtr->CycleSpeed * GetScale3D().X;
		// if (SpeedSizeDelta < 0.f)
		// {
		// 	SpeedSizeDelta = FMath::Clamp(SpeedSizeDelta + DeltaTime * MovementData.SettingsPtr->CycleAccelerationSize * GetScale3D().X, SpeedSizeDelta, 0.f);
		// }
		// else
		// {
		// 	SpeedSizeDelta = FMath::Clamp(SpeedSizeDelta - DeltaTime * MovementData.SettingsPtr->CycleAccelerationSize * GetScale3D().X, 0.f, SpeedSizeDelta);
		// }
		GetCharacterMovement()->Velocity = MovementDirectionWS * (MovementData.SettingsPtr->CycleSpeed * GetScale3D().X /*+ SpeedSizeDelta */);
		GetCharacterMovement()->SafeMoveUpdatedComponent(GetCharacterMovement()->Velocity * DeltaTime, GetCharacter()->GetActorQuat(), true, UpdateMovementHitResult);
		MovementData.LastUpdatedRootLocationRS = ComponentTransformWS.InverseTransformPosition(GetCharacter()->GetActorLocation());
	}
	else if (MovementData.WallRunState == EVSWallRunState::Ending)
	{
		const float ForwardMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.EndAnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), MovementData.EndMovementElapsedTime);
		const float SideMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.EndAnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), MovementData.EndMovementElapsedTime);
		const float UpMovementCurveValue = UVSAnimationLibrary::GetAnimationCurveValueAtTime(MovementData.EndAnimPtr->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), MovementData.EndMovementElapsedTime);
		
		FVector TargetRootLocationWS = GetRootLocation();
		TargetRootLocationWS += (ForwardMovementCurveValue - MovementData.LastUpdatedEndMovementCurveValues.X) * MovementDirectionWS * GetCharacter()->GetActorScale3D().X;
		TargetRootLocationWS += (SideMovementCurveValue - MovementData.LastUpdatedEndMovementCurveValues.Y) * ComponentTransformWS.TransformVectorNoScale(-MovementData.LastTracedWallNormalRS) * GetCharacter()->GetActorScale3D().Y;
		TargetRootLocationWS += (UpMovementCurveValue - MovementData.LastUpdatedEndMovementCurveValues.Z) * GetUpDirection() * GetCharacter()->GetActorScale3D().Z;
		
		const FVector& RootLocationDeltaWS = TargetRootLocationWS - RootLocationWS;
		GetCharacterMovement()->SafeMoveUpdatedComponent(TargetRootLocationWS - RootLocationWS, GetCharacter()->GetActorQuat(), true, UpdateMovementHitResult);
		GetCharacterMovement()->Velocity = RootLocationDeltaWS / DeltaTime;

		MovementData.LastUpdatedRootLocationRS = ComponentTransformWS.InverseTransformPosition(TargetRootLocationWS);
		MovementData.LastUpdatedEndMovementCurveValues = FVector(ForwardMovementCurveValue, SideMovementCurveValue, UpMovementCurveValue);

		MovementData.EndMovementElapsedTime = FMath::Clamp(MovementData.EndMovementElapsedTime + DeltaTime * MovementData.EndAnimPtr->PlayRate, 0.f, MovementData.EndAnimPtr->GetSafePlayTimeRange().Y);
	}
	
	GetMovementFeatureAgent()->OrientationAimData.Direction = MovementDirectionWS;

	/** Trace for ground. */
	FHitResult TraceGroundHitResult;

	if (bShouldTraceForSideWall)
	{
		const FVector& TraceGroundStart = GetRootLocation();
		const FVector& TraceGroundEnd = TraceGroundStart - MovementData.SettingsPtr->Limits.RootHeightToGroundThresholdRange.X * GetScale3D().Z * GetUpDirection();
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, TraceGroundHitResult, TraceGroundStart, TraceGroundEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA
		if (bDrawDebugShapes) { DrawDebugLineTraceSingle(GetWorld(), TraceGroundStart, TraceGroundEnd, EDrawDebugTrace::ForOneFrame, TraceGroundHitResult.bBlockingHit, TraceGroundHitResult, FColor::Turquoise, FColor::Silver, 3.f); }
#endif
	}

	/** Adjust distance to wall. */
	if (bShouldTraceForSideWall && TraceSideWallFootHitResult.bBlockingHit)
	{
		const float DistanceToWall = (RootLocationWS - TraceSideWallFootHitResult.ImpactPoint).ProjectOnToNormal(-TraceSideWallFootHitResult.ImpactNormal).Size();
		const float DistanceDelta = DistanceToWall - MovementData.SettingsPtr->CycleDistanceToWall * GetScale3D().Y;
		const float LaggedDistanceDelta = UVSMathLibrary::FloatInterpTo(DistanceDelta, 0.f, DeltaTime, 10.f);
		GetCharacterMovement()->SafeMoveUpdatedComponent((DistanceDelta - LaggedDistanceDelta) * -TraceSideWallFootHitResult.ImpactNormal, GetCharacter()->GetActorQuat(), false, UpdateMovementHitResult);
	}

	/** Check for movement end. */
	bool bShouldBreakMovement = false;
	bool bBreakMovementWithEnd = false;
	if (bShouldTraceForSideWall && (!TraceSideWallFootHitResult.bBlockingHit || !TraceSideWallFootHitResult.IsValidBlockingHit())) { bShouldBreakMovement = true; }
	if (bShouldTraceForSideWall && TraceGroundHitResult.bBlockingHit && MovementData.WallRunState != EVSWallRunState::Ending) { bBreakMovementWithEnd = true; }
	if (UpdateMovementHitResult.bBlockingHit) { bShouldBreakMovement = true; }
	else if (MovementData.WallRunState == EVSWallRunState::Ending && MovementData.EndMovementElapsedTime >= MovementData.EndAnimPtr->GetSafePlayTimeRange().Y) { bShouldBreakMovement = true; }
	if (bShouldBreakMovement)
	{
		StopWallRun(bBreakMovementWithEnd);
	}
}

bool UVSChrMovFeature_WallRunMovement::IsWallRunMode() const
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	return GameplayTagController ? (GameplayTagController->GetTagCount(EVSMovementMode::WallRunning) >= 1) : false;
}

void UVSChrMovFeature_WallRunMovement::TryWallRun(const TArray<FDataTableRowHandle>& SettingRows, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (SettingRows.IsEmpty() && DefaultSettingRows.IsEmpty()) return;
	
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			const bool bSucceeded = TryWallRunInternal(SettingRows);
			if (bSucceeded && NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
			{
				/** Only send server RPC if local execution succeeded.  */
				WallRun_Server(MovementData.SnappedParams, NetExecPolicies.ServerRPCPolicy);
			}
		}
		else if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			TryWallRun_Server(SettingRows, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_Authority)
	{
		TryWallRun_Server(SettingRows, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			TryWallRunInternal(SettingRows);
		}
	}
}

void UVSChrMovFeature_WallRunMovement::StopWallRun(bool bTryWithEndingMovement, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			StopWallRunInternal(bTryWithEndingMovement);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			StopWallRun_Server(bTryWithEndingMovement, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_Authority)
	{
		StopWallRun_Server(bTryWithEndingMovement, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			StopWallRunInternal(bTryWithEndingMovement);
		}
	}
}

bool UVSChrMovFeature_WallRunMovement::SuggestLaunchVelocityToOtherSideParalleledWall(FVector& OutVelocity, const FVector2D& WallDistanceRange, const float MovementDuration, const FDataTableRowHandle& SettingRow) const
{
	if (!IsWallRunMode()) return false;
	
	FVSWallRunSettings* Settings = !SettingRow.IsNull() ? SettingRow.GetRow<FVSWallRunSettings>(nullptr) : MovementData.SettingsPtr;
	if (!Settings) return false;
	
	if (WallDistanceRange.X <= 0.f || WallDistanceRange.X > WallDistanceRange.Y) return false;
	if (MovementData.WallRunState == EVSWallRunState::Starting)
	{
		const float StartReachTargetWallTime = MovementData.StartAnimPtr->HasTimeMark(AnimStartReachWallMarkName) ? MovementData.StartAnimPtr->GetMarkTime(AnimStartReachWallMarkName) : MovementData.StartAnimPtr->GetSafePlayTimeRange().Y;
		if (MovementData.StartMovementElapsedTime <= StartReachTargetWallTime) return false;
	}
	else if (MovementData.WallRunState == EVSWallRunState::Ending)
	{
		const float EndLeaveWallWallTime = MovementData.EndAnimPtr->HasTimeMark(AnimStartReachWallMarkName) ? MovementData.EndAnimPtr->GetMarkTime(AnimEndLeaveWallMarkName) : MovementData.EndAnimPtr->GetSafePlayTimeRange().Y;
		if (MovementData.EndMovementElapsedTime >= EndLeaveWallWallTime) return false;
	}

	const FTransform& ComponentTransformWS = MovementData.CachedParams.Component->GetComponentTransform();
	const FVector& RootLocationWS = GetRootLocation();
	const float CapsuleRadiusSC = GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector& VelocityX = UVSMathLibrary::VectorApplyAxes(GetVelocity(), FVector::ZeroVector, EVSVectorAxes::YZ, GetCharacter()->GetActorRotation());
	const FVector& CurrentWallNormalWS = ComponentTransformWS.TransformVectorNoScale(MovementData.LastTracedWallNormalRS);
	const FVector& CurrentWallPointWS = ComponentTransformWS.TransformPosition(MovementData.LastTracedWallPointRS);

	/** Trace for other side wall. */
	const float RadiusSC = GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FCollisionShape& RadiusTraceShape = FCollisionShape::MakeSphere(RadiusSC);
	FCollisionQueryParams TraceQueryParams;
	TraceQueryParams.AddIgnoredActor(GetCharacter());
	
	FHitResult TraceOtherSideWallHitResult;
	const FVector& TraceOtherSideWallStart = RootLocationWS + CapsuleRadiusSC * GetUpDirection();
	const FVector& TraceOtherSideWallEnd = TraceOtherSideWallStart + CurrentWallNormalWS * WallDistanceRange.Y * GetScale3D().Y + VelocityX * MovementDuration;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, TraceOtherSideWallHitResult, TraceOtherSideWallStart, TraceOtherSideWallEnd, GetCharacter()->GetActorQuat(), RadiusTraceShape, GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA
	if (bDrawDebugShapes) { DrawDebugSphereTraceSingle(GetWorld(), TraceOtherSideWallStart, TraceOtherSideWallEnd, RadiusSC, EDrawDebugTrace::ForDuration, TraceOtherSideWallHitResult.bBlockingHit, TraceOtherSideWallHitResult, FColor::Turquoise, FColor::Silver, 3.f); }
#endif
	if (!TraceOtherSideWallHitResult.IsValidBlockingHit()) { return false; }

	/** Not paralleled. */
	if (FMath::Abs(TraceOtherSideWallHitResult.ImpactNormal.Dot(MovementData.LastTracedWallNormalRS)) < UKismetMathLibrary::DegCos(87.5f)) return false;

	/** Distance not in range. */
	const float DistanceBetweenWallsWS = (TraceOtherSideWallHitResult.ImpactPoint - CurrentWallPointWS).ProjectOnToNormal(TraceOtherSideWallHitResult.ImpactNormal).Size();
	const float DistanceToCurrentWallWS = (RootLocationWS - CurrentWallPointWS).ProjectOnToNormal(TraceOtherSideWallHitResult.ImpactNormal).Size();
	const float DistanceToOtherSideWallWS = DistanceBetweenWallsWS - DistanceToCurrentWallWS;
	const float DistanceForwardMovement = (VelocityX.Size() * MovementDuration);
	if (!UKismetMathLibrary::InRange_FloatFloat(DistanceBetweenWallsWS / GetScale3D().Y, WallDistanceRange.X, WallDistanceRange.Y)) return false;
	
	/** Dichotomy. */
	float AngleL = 0.f, AngleR = UKismetMathLibrary::DegAtan(DistanceToOtherSideWallWS / DistanceForwardMovement);
	float AngleMidSide = (AngleL + AngleR) / 2.f;

	/** Side trace condition. */
	while (AngleL < AngleR)
	{
		const float HorizontalDeltaVal = DistanceForwardMovement * UKismetMathLibrary::DegTan(AngleMidSide) + Settings->WallTraceOffset.Y * GetScale3D().Y * UKismetMathLibrary::DegCos(AngleMidSide) - DistanceToOtherSideWallWS;
		if (HorizontalDeltaVal >= 0.f && FMath::IsNearlyZero(HorizontalDeltaVal, 0.01f)) { break; }
		if (HorizontalDeltaVal < 0.f) { AngleL = AngleMidSide; }
		else { AngleR = AngleMidSide; }
		AngleMidSide = (AngleL + AngleR) / 2.f;
	}

	/** Forward trace condition. */
	AngleL = 0.f;
	AngleR = UKismetMathLibrary::DegAtan(DistanceToOtherSideWallWS / DistanceForwardMovement);
	float AngleMidFwd = (AngleL + AngleR) / 2.f;
	while (AngleL < AngleR)
	{
		const float HorizontalDeltaVal = DistanceForwardMovement * UKismetMathLibrary::DegTan(AngleMidFwd) + Settings->WallTraceOffset.X * GetScale3D().X * UKismetMathLibrary::DegSin(AngleMidFwd) - DistanceToOtherSideWallWS;
		if (HorizontalDeltaVal >= 0.f && FMath::IsNearlyZero(HorizontalDeltaVal, 0.01f)) { break; }
		if (HorizontalDeltaVal < 0.f) { AngleL = AngleMidFwd; }
		else { AngleR = AngleMidFwd; }
		AngleMidFwd = (AngleL + AngleR) / 2.f;
	}
	if (Settings->WallTraceOffset.X * GetScale3D().X * UKismetMathLibrary::DegSin(AngleMidFwd) > Settings->WallTraceOffset.Y * GetScale3D().Y)
	{
		AngleMidFwd = UKismetMathLibrary::DegAtan((DistanceToOtherSideWallWS - Settings->WallTraceOffset.Y * GetScale3D().Y) / DistanceForwardMovement);
	}
	
	const float SpeedZ = 0.5f * FMath::Abs(GetCharacterMovement()->GetGravityZ()) * MovementDuration;
	
	OutVelocity = GetVelocity();
	const FVector& TargetVelocityYZ = VelocityX.Size() * UKismetMathLibrary::DegTan(FMath::Min(AngleMidFwd, AngleMidSide)) * CurrentWallNormalWS + SpeedZ * GetUpDirection();
	
	OutVelocity = UVSMathLibrary::VectorApplyAxes(OutVelocity, TargetVelocityYZ, EVSVectorAxes::YZ, GetCharacter()->GetActorRotation());
	return true;
}

void UVSChrMovFeature_WallRunMovement::SetWallRunState(const FGameplayTag& NewWallRunState)
{
	if (NewWallRunState == MovementData.WallRunState) return;
	const FGameplayTag PrevWallRunState = MovementData.WallRunState;
	MovementData.WallRunState = NewWallRunState;

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	if (GameplayTagController->GetTagCount(PrevWallRunState) > 0)
	{
		GameplayTagController->SetTagCount(PrevWallRunState, 0);
	}
	if (NewWallRunState != FGameplayTag::EmptyTag)
	{
		GameplayTagController->SetTagCount(NewWallRunState, 1);
	}
	GameplayTagController->NotifyTagsUpdated();
}

bool UVSChrMovFeature_WallRunMovement::TryWallRunInternal(const TArray<FDataTableRowHandle>& SettingRows)
{
	/** Can't move or input backward. */
	if (GetVelocityWallAdjusted2D().Dot(GetCharacter()->GetActorForwardVector()) < 0.f) return false;
	if (GetMovementInput2D().Dot(GetCharacter()->GetActorForwardVector()) < 0.f) return false;
	
	TArray<FDataTableRowHandle> SettingRowsToUse = SettingRows.IsEmpty() ? DefaultSettingRows : SettingRows;
	FVSWallRunSnappedParams SnappedParams;
	for (const FDataTableRowHandle& SettingRow : SettingRowsToUse)
	{
		if (CalcWallRunSnappedParams(SnappedParams, SettingRow))
		{
			WallRunBySnappedParams(SnappedParams);
			return true;
		}
	}
	return false;
}

void UVSChrMovFeature_WallRunMovement::WallRunBySnappedParams(const FVSWallRunSnappedParams& SnappedParams)
{
	if (SnappedParams.SettingsRow.IsNull()) return;

	MovementData.SnappedParams = SnappedParams;
	MovementData.SettingsPtr = MovementData.SnappedParams.SettingsRow.GetRow<FVSWallRunSettings>(nullptr);
	
	UPrimitiveComponent* Component = SnappedParams.Actor.IsValid() ? Cast<UPrimitiveComponent>(UVSActorLibrary::GetActorComponentByName(SnappedParams.Actor.Get(), SnappedParams.ComponentName)) : nullptr;
	if (!Component)
	{
		/** Retrace the component. */
		const FCollisionShape& SphereTraceShape = FCollisionShape::MakeSphere(1.f);
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(GetCharacter());
		FHitResult ComponentTraceHit;
		const FVector& StartFrontWallPointWS = SnappedParams.StartComponentTransform.TransformPosition(SnappedParams.StartWallPointRS);
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, ComponentTraceHit, StartFrontWallPointWS, StartFrontWallPointWS - SnappedParams.StartWallNormal2DRS * 16.f, FQuat::Identity,
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

	MovementData.CachedParams.ActionID = FMath::RandRange(0, INT16_MAX);

	const FTransform& ComponentTransformWS = MovementData.CachedParams.Component->GetComponentTransform();
	const FVector& StartRootLocationWS = GetRootLocation();
	const FVector& StartRootLocationRS = UKismetMathLibrary::InverseTransformLocation(ComponentTransformWS,StartRootLocationWS);
	
	if (FVSAnimSequenceReference* StartAnim = (!MovementData.SnappedParams.bLeftOrRight ? MovementData.SettingsPtr->LeftAnims : MovementData.SettingsPtr->RightAnims).StartAnim.GetRow<FVSAnimSequenceReference>(nullptr))
	{
		if (StartAnim->IsValid())
		{
			const FVector2D& AnimSafePlayTimeRange = StartAnim->GetSafePlayTimeRange();
			const float ReachTargetWallTime = StartAnim->HasTimeMark(AnimStartReachWallMarkName) ? StartAnim->GetMarkTime(AnimStartReachWallMarkName) : StartAnim->GetSafePlayTimeRange().Y;
			MovementData.StartAnimPtr = StartAnim;
			MovementData.CachedParams.AnimStartMovementCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), AnimSafePlayTimeRange.X);
			MovementData.CachedParams.AnimStartMovementCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), AnimSafePlayTimeRange.X);
			MovementData.CachedParams.AnimStartMovementCurveValues.Z = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), AnimSafePlayTimeRange.X);
			MovementData.CachedParams.AnimStartReachWallMovementCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), ReachTargetWallTime);
			MovementData.CachedParams.AnimStartReachWallMovementCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), ReachTargetWallTime);
			MovementData.CachedParams.AnimStartReachWallMovementCurveValues.Z = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), ReachTargetWallTime);
			MovementData.CachedParams.AnimStartSettleMovementCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::X), AnimSafePlayTimeRange.Y);
			MovementData.CachedParams.AnimStartSettleMovementCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), AnimSafePlayTimeRange.Y);
			MovementData.CachedParams.AnimStartSettleMovementCurveValues.Z = UVSAnimationLibrary::GetAnimationCurveValueAtTime(StartAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), AnimSafePlayTimeRange.Y);
			
			const FVector& StartWallRootLocationWS = ComponentTransformWS.TransformPosition(MovementData.SnappedParams.StartWallRootPointRS);
			const FVector& StartWallNormalWS = ComponentTransformWS.TransformVectorNoScale(MovementData.SnappedParams.StartWallNormal2DRS);
			FVector StartReachWallRootLocationWS = StartWallRootLocationWS + MovementData.SettingsPtr->CycleDistanceToWall * GetScale3D().Y * StartWallNormalWS;
			StartReachWallRootLocationWS += StartWallNormalWS * FMath::Abs(MovementData.CachedParams.AnimStartSettleMovementCurveValues.Y - MovementData.CachedParams.AnimStartReachWallMovementCurveValues.Y);

			MovementData.CachedParams.StartReachWallRootLocationRS = ComponentTransformWS.InverseTransformPosition(StartReachWallRootLocationWS);
			MovementData.LastUpdatedStartMovementCurveValues = MovementData.CachedParams.AnimStartMovementCurveValues;
			MovementData.StartMovementElapsedTime = AnimSafePlayTimeRange.X;
		}
	}
	if (FVSAnimSequenceReference* EndAnim = (!MovementData.SnappedParams.bLeftOrRight ? MovementData.SettingsPtr->LeftAnims : MovementData.SettingsPtr->RightAnims).EndAnim.GetRow<FVSAnimSequenceReference>(nullptr))
	{
		if (EndAnim->IsValid())
		{
			MovementData.EndAnimPtr = EndAnim;
			const FVector2D& AnimSafePlayTimeRange = EndAnim->GetSafePlayTimeRange();
			MovementData.LastUpdatedEndMovementCurveValues.X = UVSAnimationLibrary::GetAnimationCurveValueAtTime(EndAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), AnimSafePlayTimeRange.X);
			MovementData.LastUpdatedEndMovementCurveValues.Y = UVSAnimationLibrary::GetAnimationCurveValueAtTime(EndAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Y), AnimSafePlayTimeRange.X);
			MovementData.LastUpdatedEndMovementCurveValues.Z = UVSAnimationLibrary::GetAnimationCurveValueAtTime(EndAnim->AnimSequence, UVSMovementSystemSettings::Get()->AnimMovementCurveNames.FindRef(EAxis::Z), AnimSafePlayTimeRange.X);
			MovementData.EndMovementElapsedTime = AnimSafePlayTimeRange.X;
		}
	}
	
	MovementData.CachedParams.StartRootLocationRS = StartRootLocationRS;
	MovementData.LastUpdatedRootLocationRS = StartRootLocationRS;
	MovementData.LastTracedWallNormalRS = SnappedParams.StartWallNormal2DRS;
	MovementData.LastTracedWallPointRS = SnappedParams.StartWallPointRS;
	
	if (!IsWallRunMode())
	{
		SetMovementMode(EVSMovementMode::WallRunning, false);
	}
	const float TargetHalfHeightUSC = MovementData.SettingsPtr->CapsuleHalfHeight > 0.f ? MovementData.SettingsPtr->CapsuleHalfHeight : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MovementData.CapsuleHalfHeightOffsetUSCZ = TargetHalfHeightUSC - GetMovementCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(TargetHalfHeightUSC);
	GetCharacterMovement()->StopMovementImmediately();
	SetWallRunState((SnappedParams.bFromGround && MovementData.StartAnimPtr) ? EVSWallRunState::Starting : EVSWallRunState::Cycling);
}

void UVSChrMovFeature_WallRunMovement::StopWallRunInternal(bool bTryEndMovement)
{
	bool bCanDoEndMovement = bTryEndMovement && MovementData.EndAnimPtr && MovementData.WallRunState != EVSWallRunState::Ending;
	if (bCanDoEndMovement)
	{
		if (MovementData.WallRunState == EVSWallRunState::Starting)
		{
			const float StartReachTargetWallTime = MovementData.StartAnimPtr->HasTimeMark(AnimStartReachWallMarkName) ? MovementData.StartAnimPtr->GetMarkTime(AnimStartReachWallMarkName) : MovementData.StartAnimPtr->GetSafePlayTimeRange().Y;
			if (MovementData.StartMovementElapsedTime < StartReachTargetWallTime)
			{
				bCanDoEndMovement = false;
			}
		}
	}
	
	if (!bCanDoEndMovement)
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
		
		if (IsWallRunMode())
		{
			SetMovementMode(UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()) ? EVSMovementMode::Walking : EVSMovementMode::Falling, false);
		}
		SetWallRunState(FGameplayTag::EmptyTag);
	}
	else
	{
		MovementData.WallRunState = EVSWallRunState::Ending;
	}
}

bool UVSChrMovFeature_WallRunMovement::CalcWallRunSnappedParams(FVSWallRunSnappedParams& OutSnappedParams, const FDataTableRowHandle& SettingsRow) const
{
	FVSWallRunSettings* Settings = SettingsRow.GetRow<FVSWallRunSettings>(nullptr);
	if (!Settings || !Settings->IsValid()) return false;
	
	if (Settings->Limits.bRequireMovementInput2D && !HasMovementInput2D()) return false;
	
	const bool bFromGround = UVSActorLibrary::IsCharacterOnWalkableFloor(GetCharacter()) && GetVelocityZ().IsNearlyZero(0.01f);

	/** SC means scaled. */
	const FVector& CharacterScaleWS = GetCharacter()->GetActorScale();
	const FVector& RootLocationWS = GetRootLocation();
	const float RadiusSC = GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float TargetHalfHeightSC = Settings->CapsuleHalfHeight > 0.f ? (Settings->CapsuleHalfHeight * GetScale3D().Z) : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FCollisionShape& RootFootRadiusTraceShape = FCollisionShape::MakeSphere(FMath::Min(RadiusSC, Settings->RootFootHeightOffset * GetScale3D().Z));
	const FCollisionShape& CapsuleTraceShape = FCollisionShape::MakeCapsule(RadiusSC, TargetHalfHeightSC);
	FCollisionQueryParams TraceQueryParams;
	TraceQueryParams.AddIgnoredActor(GetCharacter());
	
	FVector WallTraceDirection = GetCharacter()->GetActorForwardVector();
	if (bFromGround)
	{
		if (HasMovementInput2D()) { WallTraceDirection = GetMovementInput2D().GetSafeNormal(); }
		else if (IsMoving2D()) { WallTraceDirection = GetVelocityWallAdjusted2D().GetSafeNormal(); }
	}
	else
	{
		if (IsMoving2D()) { WallTraceDirection = GetVelocityWallAdjusted2D().GetSafeNormal(); }
		else if (HasMovementInput2D()) { WallTraceDirection = GetMovementInput2D().GetSafeNormal(); }
	}

	/** Trace for ground. */
	const FVector& TraceGroundStart = RootLocationWS;
	const FVector& TraceGroundEnd = TraceGroundStart - Settings->Limits.RootHeightToGroundThresholdRange.Y * GetScale3D().Z * GetUpDirection();
	FHitResult TraceGroundHitResult;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, TraceGroundHitResult, TraceGroundStart, TraceGroundEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA
	if (bDrawDebugShapes) { DrawDebugLineTraceSingle(GetWorld(), TraceGroundStart, TraceGroundEnd, EDrawDebugTrace::ForOneFrame, TraceGroundHitResult.bBlockingHit, TraceGroundHitResult, FColor::Orange, FColor::Purple, 3.f); }
#endif
	if (TraceGroundHitResult.bBlockingHit)
	{
		/** The ground is too close. */
		if (!bFromGround && (RootLocationWS - TraceGroundHitResult.ImpactPoint + (bFromGround ? Settings->WallTraceOffset.Z * GetScale3D().Z : 0.f) * GetUpDirection()).ProjectOnToNormal(GetUpDirection()).Size() < Settings->Limits.RootHeightToGroundThresholdRange.Y * GetScale3D().Z)
		{
			return false;
		}
	}

	/** Trace forward to find a possible wall. */
	const FVector& WallForwardTraceStart = RootLocationWS + Settings->RootFootHeightOffset * GetScale3D().Z + (bFromGround ? Settings->WallTraceOffset.Z * CharacterScaleWS.Z * GetCharacter()->GetActorUpVector() : FVector::ZeroVector);
	const FVector& WallForwardTraceEnd = WallForwardTraceStart + Settings->WallTraceOffset.X * CharacterScaleWS.X * WallTraceDirection;
	FHitResult WallForwardTraceHitResult;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, WallForwardTraceHitResult, WallForwardTraceStart, WallForwardTraceEnd, GetCharacter()->GetActorQuat(), RootFootRadiusTraceShape, GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (bDrawDebugShapes) DrawDebugSphereTraceSingle(GetWorld(), WallForwardTraceStart, WallForwardTraceEnd, RadiusSC, EDrawDebugTrace::ForOneFrame, WallForwardTraceHitResult.bBlockingHit, WallForwardTraceHitResult, FColor::Red, FColor::Green, 3.f);
#endif
	if (WallForwardTraceHitResult.bBlockingHit && !WallForwardTraceHitResult.IsValidBlockingHit()) return false;

	/** If has forward traced wall, then adjust the forward direction, and trace for side. */
	FVector WallTraceAdjustedDirectionWS = !WallForwardTraceHitResult.bBlockingHit ? WallTraceDirection : GetUpDirection().Cross(WallForwardTraceHitResult.Normal).GetSafeNormal();
	if (WallTraceAdjustedDirectionWS.Dot(WallTraceDirection) < 0.f) { WallTraceAdjustedDirectionWS *= -1.f; }
	
	/** Trace side to find a wall at the foot. */
	/** Left side trace. */
	const FVector& WallTraceLeftDirection = GetCharacter()->GetActorUpVector().Cross(WallTraceAdjustedDirectionWS).GetSafeNormal();
	const FVector& WallTraceLeftSideStart = WallForwardTraceStart + WallTraceAdjustedDirectionWS * (bFromGround ? Settings->WallTraceOffset.X * CharacterScaleWS.X : RadiusSC);
	const FVector& WallTraceLeftSideEnd = WallTraceLeftSideStart - Settings->WallTraceOffset.Y * CharacterScaleWS.Y * WallTraceLeftDirection;
	FHitResult WallTraceLeftSideHitResult;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, WallTraceLeftSideHitResult, WallTraceLeftSideStart, WallTraceLeftSideEnd, GetCharacter()->GetActorQuat(), RootFootRadiusTraceShape, GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (bDrawDebugShapes) DrawDebugSphereTraceSingle(GetWorld(), WallTraceLeftSideStart, WallTraceLeftSideEnd, RadiusSC, EDrawDebugTrace::ForOneFrame, WallTraceLeftSideHitResult.bBlockingHit, WallTraceLeftSideHitResult, FColor::Blue, FColor::Yellow, 3.f);
#endif
	bool bValidWallLeftSide = true;
	if (!WallTraceLeftSideHitResult.IsValidBlockingHit()) { bValidWallLeftSide = false; }
	const FVector& MovementDirectionLeft = FVector::VectorPlaneProject(WallTraceLeftSideHitResult.ImpactNormal.Cross(GetUpDirection()), GetUpDirection()).GetSafeNormal();
	const float FacingLeftMovementAngle = (FMath::RadiansToDegrees(FQuat::FindBetweenVectors(MovementDirectionLeft, GetCharacter()->GetActorForwardVector()).GetAngle())) * (FMath::Sign(GetCharacter()->GetActorForwardVector().Dot(-WallTraceLeftSideHitResult.ImpactNormal)));
	if (bValidWallLeftSide && !Settings->Limits.WallComponentQuery.Matches(WallTraceLeftSideHitResult.GetComponent())) { bValidWallLeftSide = false; }
	if (bValidWallLeftSide && (WallTraceLeftSideHitResult.ImpactPoint - RootLocationWS).ProjectOnToNormal(WallTraceLeftSideHitResult.ImpactNormal).Size() > Settings->WallTraceOffset.Y * CharacterScaleWS.Y) { bValidWallLeftSide = false; }
	if (!UKismetMathLibrary::InRange_FloatFloat(FacingLeftMovementAngle, Settings->Limits.FacingMovementAngleRange2D.X, Settings->Limits.FacingMovementAngleRange2D.Y)) { bValidWallLeftSide = false; }
	if (bValidWallLeftSide && IsMoving2D())
	{
		const float VelocityMovementAngle = (FMath::RadiansToDegrees(FQuat::FindBetweenVectors(MovementDirectionLeft, GetVelocityWallAdjusted2D()).GetAngle())) * (FMath::Sign(GetVelocityWallAdjusted2D().Dot(-WallTraceLeftSideHitResult.ImpactNormal)));
		if (!UKismetMathLibrary::InRange_FloatFloat(VelocityMovementAngle, Settings->Limits.VelocityTowardsMovementAngleRange2D.X, Settings->Limits.VelocityTowardsMovementAngleRange2D.Y)) { bValidWallLeftSide = false; }
	}
	if (bValidWallLeftSide && HasMovementInput2D())
	{	const float InputMovementAngle = (FMath::RadiansToDegrees(FQuat::FindBetweenVectors(MovementDirectionLeft, GetMovementInput2D()).GetAngle())) * (FMath::Sign(GetMovementInput2D().Dot(-WallTraceLeftSideHitResult.ImpactNormal)));
		if (!UKismetMathLibrary::InRange_FloatFloat(InputMovementAngle, Settings->Limits.InputTowardsMovementAngleRange2D.X, Settings->Limits.InputTowardsMovementAngleRange2D.Y)) { bValidWallLeftSide = false; }
	}
	/** Check for block. */
	if (bValidWallLeftSide)
	{
		const FVector& FinalRootLocation = WallTraceLeftSideHitResult.ImpactPoint - Settings->RootFootHeightOffset * GetScale3D().Z * GetUpDirection() + WallTraceLeftSideHitResult.ImpactNormal * FMath::Max(Settings->CycleDistanceToWall, RadiusSC);
		const FVector& BlockCheckStart = RootLocationWS + TargetHalfHeightSC * GetUpDirection();
		const FVector& BlockCheckEnd = FinalRootLocation + TargetHalfHeightSC * GetUpDirection();
		FHitResult BlockHitResult;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, BlockHitResult, BlockCheckStart, BlockCheckEnd, GetCharacter()->GetActorQuat(), CapsuleTraceShape, GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
		if (bDrawDebugShapes) DrawDebugCapsuleTraceSingle(GetWorld(), BlockCheckStart, BlockCheckEnd, RadiusSC, TargetHalfHeightSC, EDrawDebugTrace::ForOneFrame, BlockHitResult.bBlockingHit, BlockHitResult, FColor::Cyan, FColor::Magenta, 3.f);
#endif
		if (BlockHitResult.bBlockingHit) { bValidWallLeftSide = false; }
	}
	/** Check for hand position side. */
	if (bValidWallLeftSide)
	{
		const FVector& FinalWallRootLocation = WallTraceLeftSideHitResult.ImpactPoint - Settings->RootFootHeightOffset * GetScale3D().Z * GetUpDirection();
		const FVector& HandSideCheckStart = FinalWallRootLocation + Settings->RootHandHeightOffset * GetScale3D().Z * GetUpDirection() + WallTraceLeftSideHitResult.ImpactNormal;
		const FVector& HandSideCheckEnd = HandSideCheckStart - 2.f * WallTraceLeftSideHitResult.ImpactNormal;
		FHitResult HandSideHitResult;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, HandSideHitResult, HandSideCheckStart, HandSideCheckEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
		if (bDrawDebugShapes) DrawDebugLineTraceSingle(GetWorld(), HandSideCheckStart, HandSideCheckEnd, EDrawDebugTrace::ForOneFrame, HandSideHitResult.bBlockingHit, HandSideHitResult, FColor::Orange, FColor::Purple, 3.f);
#endif
		if (!HandSideHitResult.IsValidBlockingHit()) { bValidWallLeftSide = false; }
	}
	
	/** Right side trace. */
	const FVector& WallTraceRightDirection = -WallTraceLeftDirection;
	const FVector& WallTraceRightSideStart = WallTraceLeftSideStart;
	const FVector& WallTraceRightSideEnd = WallTraceRightSideStart - Settings->WallTraceOffset.Y * CharacterScaleWS.Y * WallTraceRightDirection;
	FHitResult WallTraceRightSideHitResult;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, WallTraceRightSideHitResult, WallTraceRightSideStart, WallTraceRightSideEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (bDrawDebugShapes) DrawDebugLineTraceSingle(GetWorld(), WallTraceRightSideStart, WallTraceRightSideEnd, EDrawDebugTrace::ForOneFrame, WallTraceRightSideHitResult.bBlockingHit, WallTraceRightSideHitResult, FColor::Blue, FColor::Yellow, 3.f);
#endif
	
	bool bValidWallRightSide = true;
	if (!WallTraceRightSideHitResult.IsValidBlockingHit()) { bValidWallRightSide = false; }
	const FVector& MovementDirectionRight = FVector::VectorPlaneProject(GetUpDirection().Cross(WallTraceRightSideHitResult.ImpactNormal), GetUpDirection()).GetSafeNormal();
	const float FacingRightMovementAngle = (FMath::RadiansToDegrees(FQuat::FindBetweenVectors(MovementDirectionRight, GetCharacter()->GetActorForwardVector()).GetAngle())) * (FMath::Sign(GetCharacter()->GetActorForwardVector().Dot(-WallTraceRightSideHitResult.ImpactNormal)));
	if (bValidWallRightSide && !Settings->Limits.WallComponentQuery.Matches(WallTraceRightSideHitResult.GetComponent())) { bValidWallRightSide = false; }
	if (bValidWallRightSide && (WallTraceRightSideHitResult.ImpactPoint - RootLocationWS).ProjectOnToNormal(WallTraceRightSideHitResult.ImpactNormal).Size() > Settings->WallTraceOffset.Y * CharacterScaleWS.Y) { bValidWallRightSide = false; }
	if (!UKismetMathLibrary::InRange_FloatFloat(FacingRightMovementAngle, Settings->Limits.FacingMovementAngleRange2D.X, Settings->Limits.FacingMovementAngleRange2D.Y)) { bValidWallRightSide = false; }
	if (bValidWallRightSide && IsMoving2D())
	{
		const float VelocityMovementAngle = (FMath::RadiansToDegrees(FQuat::FindBetweenVectors(MovementDirectionRight, GetVelocityWallAdjusted2D()).GetAngle())) * (FMath::Sign(GetVelocityWallAdjusted2D().Dot(-WallTraceRightSideHitResult.ImpactNormal)));
		if (!UKismetMathLibrary::InRange_FloatFloat(VelocityMovementAngle, Settings->Limits.VelocityTowardsMovementAngleRange2D.X, Settings->Limits.VelocityTowardsMovementAngleRange2D.Y)) { bValidWallRightSide = false; }
	}
	if (bValidWallRightSide && HasMovementInput2D())
	{
		const float InputMovementAngle = (FMath::RadiansToDegrees(FQuat::FindBetweenVectors(MovementDirectionRight, GetMovementInput2D()).GetAngle())) * (FMath::Sign(GetMovementInput2D().Dot(-WallTraceRightSideHitResult.ImpactNormal)));
		if (!UKismetMathLibrary::InRange_FloatFloat(InputMovementAngle, Settings->Limits.InputTowardsMovementAngleRange2D.X, Settings->Limits.InputTowardsMovementAngleRange2D.Y)) { bValidWallRightSide = false; }
	}
	/** Check for block. */
	if (bValidWallRightSide)
	{
		const FVector& FinalRootLocation = WallTraceRightSideHitResult.ImpactPoint - Settings->RootFootHeightOffset * GetScale3D().Z * GetUpDirection() + WallTraceRightSideHitResult.ImpactNormal * FMath::Max(Settings->CycleDistanceToWall, RadiusSC);
		const FVector& BlockCheckStart = RootLocationWS + TargetHalfHeightSC * GetUpDirection();
		const FVector& BlockCheckEnd = FinalRootLocation + TargetHalfHeightSC * GetUpDirection();
		FHitResult BlockHitResult;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, BlockHitResult, BlockCheckStart, BlockCheckEnd, GetCharacter()->GetActorQuat(), CapsuleTraceShape, GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
		if (bDrawDebugShapes) DrawDebugCapsuleTraceSingle(GetWorld(), BlockCheckStart, BlockCheckEnd, RadiusSC, TargetHalfHeightSC, EDrawDebugTrace::ForDuration, BlockHitResult.bBlockingHit, BlockHitResult, FColor::Cyan, FColor::Magenta, 3.f);
#endif
		if (BlockHitResult.bBlockingHit) { bValidWallRightSide = false; }
	}
	/** Check for hand position side. */
	if (bValidWallRightSide)
	{
		const FVector& FinalWallRootLocation = WallTraceRightSideHitResult.ImpactPoint - Settings->RootFootHeightOffset * GetScale3D().Z * GetUpDirection();
		const FVector& HandSideCheckStart = FinalWallRootLocation + Settings->RootHandHeightOffset * GetScale3D().Z * GetUpDirection() + WallTraceRightSideHitResult.ImpactNormal;
		const FVector& HandSideCheckEnd = HandSideCheckStart - 2.f * WallTraceRightSideHitResult.ImpactNormal;
		FHitResult HandSideHitResult;
		UVSGameplayLibrary::SweepSingleByShapeAndChannels(this, HandSideHitResult, HandSideCheckStart, HandSideCheckEnd, GetCharacter()->GetActorQuat(), FCollisionShape(), GetCharacter()->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceQueryParams);
#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
		if (bDrawDebugShapes) DrawDebugLineTraceSingle(GetWorld(), HandSideCheckStart, HandSideCheckEnd, EDrawDebugTrace::ForDuration, HandSideHitResult.bBlockingHit, HandSideHitResult, FColor::Orange, FColor::Purple, 3.f);
#endif
		if (!HandSideHitResult.IsValidBlockingHit()) { bValidWallRightSide = false; }
	}
	
	/** No valid wall. */
	if (!bValidWallLeftSide && !bValidWallRightSide) return false;
	
	/** Left : false. */
	bool bLeftOrRight = !bValidWallLeftSide;
	if (bValidWallLeftSide && bValidWallRightSide)
	{
		const float DistanceLeftSide = (WallTraceLeftSideHitResult.ImpactPoint - GetCharacter()->GetActorLocation()).ProjectOnToNormal(GetCharacter()->GetActorRightVector()).Size();
		const float DistanceRightSide = (WallTraceRightSideHitResult.ImpactPoint - GetCharacter()->GetActorLocation()).ProjectOnToNormal(GetCharacter()->GetActorRightVector()).Size();
		if (DistanceRightSide < DistanceLeftSide) bLeftOrRight = true;
	}
	FHitResult* WallHitResult = !bLeftOrRight ? &WallTraceLeftSideHitResult : &WallTraceRightSideHitResult;

	const FTransform& ComponentTransformWS = WallHitResult->Component->GetComponentTransform();
	OutSnappedParams.SettingsRow = SettingsRow;
	OutSnappedParams.Actor = WallHitResult->GetActor();
	OutSnappedParams.ComponentName = WallHitResult->GetComponent()->GetFName();
	OutSnappedParams.StartComponentTransform = ComponentTransformWS;
	OutSnappedParams.StartWallNormal2DRS = ComponentTransformWS.InverseTransformVectorNoScale(WallHitResult->ImpactNormal);
	OutSnappedParams.StartWallPointRS = ComponentTransformWS.InverseTransformPosition(WallHitResult->ImpactPoint);
	OutSnappedParams.StartWallRootPointRS = ComponentTransformWS.InverseTransformPosition(WallHitResult->ImpactPoint - Settings->RootFootHeightOffset * GetUpDirection());
	OutSnappedParams.StartMovementDirection2DRS = ComponentTransformWS.InverseTransformVectorNoScale(!bLeftOrRight ? MovementDirectionLeft : MovementDirectionRight);
	OutSnappedParams.ServerSideServerStartTime = UVSGameplayLibrary::GetServerTimeSeconds(this);
	OutSnappedParams.bLeftOrRight = bLeftOrRight;
	OutSnappedParams.bFromGround = bFromGround;
	
	return true;
}

void UVSChrMovFeature_WallRunMovement::TryWallRun_Server_Implementation(const TArray<FDataTableRowHandle>& SettingRows, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		bool bSuccessful = TryWallRunInternal(SettingRows);
		if (bSuccessful && NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			WallRun_Multicast(MovementData.SnappedParams, NetExecPolicy);
		}
	}
}

void UVSChrMovFeature_WallRunMovement::WallRun_Server_Implementation(const FVSWallRunSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		WallRunBySnappedParams(SnappedParams);
	}
	if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
	{
		WallRun_Multicast(MovementData.SnappedParams, NetExecPolicy);
	}
}

void UVSChrMovFeature_WallRunMovement::WallRun_Multicast_Implementation(const FVSWallRunSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;
	if (SnappedParams.SettingsRow.IsNull()) { bShouldExecute = false; }

	/** Authority already handled. */
	if (GetCharacter()->HasAuthority()) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		WallRunBySnappedParams(SnappedParams);
	}
}

void UVSChrMovFeature_WallRunMovement::StopWallRun_Server_Implementation(bool bTryEndMovement, EVSNetAuthorityMethodExecPolicy::Type NetPolicy)
{
	if (NetPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		StopWallRunInternal(bTryEndMovement);
	}
	if (NetPolicy > EVSNetAuthorityMethodExecPolicy::Server)
	{
		StopWallRun_Multicast(bTryEndMovement, NetPolicy);
	}
}

void UVSChrMovFeature_WallRunMovement::StopWallRun_Multicast_Implementation(bool bTryEndMovement, EVSNetAuthorityMethodExecPolicy::Type NetPolicy)
{
	bool bShouldExecute = true;

	/** Authority already handled. */
	if (GetCharacter()->HasAuthority()) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy && !(NetPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy && !(NetPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		StopWallRunInternal(bTryEndMovement);
	}
}


