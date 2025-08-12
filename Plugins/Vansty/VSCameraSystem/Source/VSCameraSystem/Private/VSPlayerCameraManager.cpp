// Copyright VanstySanic. All Rights Reserved.

#include "VSPlayerCameraManager.h"
#include "VSCameraSystemUtils.h"
#include "Camera/CameraModifier.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"

AVSPlayerCameraManager::AVSPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ViewRotationAxesLimit = EVSRotatorAxes::RollPitchYaw;
}

void AVSPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	for( int32 ModifierIdx = 0; ModifierIdx < ModifierList.Num(); ModifierIdx++ )
	{
		if( ModifierList[ModifierIdx] != NULL && 
			!ModifierList[ModifierIdx]->IsDisabled() )
		{
			if( ModifierList[ModifierIdx]->ProcessViewRotation(ViewTarget.Target, DeltaTime, OutViewRotation, OutDeltaRot) )
			{
				break;
			}
		}
	}

	if (!GetViewTarget()) return;
	const FRotator& Space = GetViewTarget()->GetActorRotation();
	FRotator OutViewRotationRS = UKismetMathLibrary::InverseTransformRotation(FTransform(Space), OutViewRotation);

	OutViewRotationRS += OutDeltaRot;
	OutDeltaRot = FRotator::ZeroRotator;

	OutViewRotationRS.Normalize();
	OutViewRotationRS.Roll = 0.f;
	
	const bool bIsHeadTrackingAllowed = false;
	// GEngine->XRSystem.IsValid() &&
	// (GetWorld() != nullptr ? GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()) : GEngine->XRSystem->IsHeadTrackingAllowed());
	if(bIsHeadTrackingAllowed)
	{
		OutViewRotationRS.Normalize();
	}
	else
	{
		if (ViewRotationAxesLimit & EVSRotatorAxes::Pitch) { LimitViewPitch( OutViewRotationRS, ViewPitchMin, ViewPitchMax ); }
		if (ViewRotationAxesLimit & EVSRotatorAxes::Yaw) { LimitViewYaw( OutViewRotationRS, ViewYawMin, ViewYawMax ); }
		if (ViewRotationAxesLimit & EVSRotatorAxes::Roll) { LimitViewRoll( OutViewRotationRS, ViewRollMin, ViewRollMax ); }
	}
	
	
	OutViewRotation = UKismetMathLibrary::TransformRotation(FTransform(Space), OutViewRotationRS);
}
