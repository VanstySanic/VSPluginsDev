// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "VSMovementSystemSettings.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Types/VSChrMovOrientationTypes.h"

UVSChrMovFeature_OrientationEvaluator::UVSChrMovFeature_OrientationEvaluator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSChrMovFeature_OrientationEvaluator::EvaluateOrientation_Implementation(FRotator& OutRotation, const FVSOrientationEvaluateParams& Params)
{
	const FVector& UpDirection =
		Params.NamedParams.VectorParams.Contains(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.UpDirection)
		? Params.NamedParams.VectorParams.FindRef(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.UpDirection)
		: GetUpDirection();
	const FQuat& WorldToDownVectorRotation = FQuat::FindBetweenNormals(FVector::UpVector, UpDirection);
	const FQuat& DownVectorToWorldRotation = WorldToDownVectorRotation.Inverse();
	const FRotator& CharacterRotationWS = GetCharacter()->GetActorRotation();
	FRotator CharacterRotationDS = UKismetMathLibrary::ComposeRotators(CharacterRotationWS, DownVectorToWorldRotation.Rotator());
	if (Params.bReturnRotationInSpace2D) CharacterRotationDS.SetComponentForAxis(EAxis::Y, 0.f);
	CharacterRotationDS.Normalize();
	OutRotation = UKismetMathLibrary::ComposeRotators(CharacterRotationDS, WorldToDownVectorRotation.Rotator());
	
	return false;
}



UVSLS_ChrMovFeature_OrientationEvaluator_Common::UVSLS_ChrMovFeature_OrientationEvaluator_Common(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSLS_ChrMovFeature_OrientationEvaluator_Common::EvaluateOrientation_Implementation(FRotator& OutRotation, const FVSOrientationEvaluateParams& Params)
{
	Super::EvaluateOrientation_Implementation(OutRotation, Params);
	if (!GetCharacter() || Params.Type == EVSOrientationEvaluateType::None) return false;
	
	const FVector& UpDirection =
		Params.NamedParams.VectorParams.Contains(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.UpDirection)
		? Params.NamedParams.VectorParams.FindRef(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.UpDirection)
		: GetUpDirection();
	
	const FQuat& WorldToUpDirectionRotation = FQuat::FindBetweenNormals(FVector::UpVector, UpDirection);
	const FQuat& UpDirectionToWorldRotation = WorldToUpDirectionRotation.Inverse();
	
	/** Movement related. */
	if (Params.Type == EVSOrientationEvaluateType::Velocity
		|| Params.Type == EVSOrientationEvaluateType::Input
		|| Params.Type == EVSOrientationEvaluateType::Movement)
	{
		const FVector& VelocityToProcess = Params.NamedParams.VectorParams.Contains(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.Velocity)
			? Params.NamedParams.VectorParams.FindRef(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.Velocity)
			: GetVelocity();
		
		const FVector& InputToProcess = Params.NamedParams.VectorParams.Contains(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.MovementInput)
			? Params.NamedParams.VectorParams.FindRef(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.MovementInput)
			: GetMovementInput();
		
		FVector VelocityGS = UKismetMathLibrary::Quat_RotateVector(UpDirectionToWorldRotation, VelocityToProcess);
		FVector InputGS = UKismetMathLibrary::Quat_RotateVector(UpDirectionToWorldRotation, InputToProcess);
		if (Params.bReturnRotationInSpace2D) VelocityGS.Z = 0.f;
		if (Params.bReturnRotationInSpace2D) InputGS.Z = 0.f;

		FVector AdjustedVelocityDirectionDS =
			(bAdjustMovementAgainstWall2D && Params.bMovementAgainstWallAdjustment2D && !InputGS.IsNearlyZero(0.01f) && IsMovingAgainstWall2D())
			? InputGS : VelocityGS;

		if (VelocityGS.IsNearlyZero(0.01f) && InputGS.IsNearlyZero(0.01f)) return false;
		
		if (Params.Type == EVSOrientationEvaluateType::Velocity)
		{
			if (VelocityGS.IsNearlyZero(0.01f)) return false;
			OutRotation = UKismetMathLibrary::ComposeRotators(AdjustedVelocityDirectionDS.Rotation(), UpDirectionToWorldRotation.Rotator());
			return true;
		}
		if (Params.Type == EVSOrientationEvaluateType::Input)
		{
			if (InputGS.IsNearlyZero(0.01f)) return false;
			OutRotation = UKismetMathLibrary::ComposeRotators(InputGS.Rotation(), UpDirectionToWorldRotation.Rotator());
			return true;
		}
		
		OutRotation = UKismetMathLibrary::ComposeRotators(AdjustedVelocityDirectionDS.IsNearlyZero(0.01f) ? InputGS.Rotation() : AdjustedVelocityDirectionDS.Rotation(), UpDirectionToWorldRotation.Rotator());
		return true;
	}


	/** Aiming. */
	if (Params.Type == EVSOrientationEvaluateType::Aiming)
	{
		FVector TargetPoint = Params.NamedParams.VectorParams.Contains(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.AimTargetPoint)
			? Params.NamedParams.VectorParams.FindRef(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.AimTargetPoint) : AimTargetPoint;
		
		USceneComponent* ComponentToUse = Params.NamedParams.VectorParams.Contains(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.AimTargetComponent)
			? Params.NamedParams.ComponentParams.FindRef(UVSMovementSystemSettings::Get()->OrientationEvaluateCommonParamNames.AimTargetComponent) : AimingTargetComponent.Get();
		if (ComponentToUse) { TargetPoint = ComponentToUse->GetComponentLocation(); }
		
		FVector AimingDirectionGS = UKismetMathLibrary::Quat_RotateVector(UpDirectionToWorldRotation, TargetPoint - GetCharacter()->GetActorLocation());
		if (Params.bReturnRotationInSpace2D) AimingDirectionGS.Z = 0.f;

		OutRotation = UKismetMathLibrary::ComposeRotators(AimingDirectionGS.Rotation(), WorldToUpDirectionRotation.Rotator());
		return true;
	}
	
	/** Control and view. */
	if (Params.Type == EVSOrientationEvaluateType::Control)
	{
		FRotator AnsRotationDS = UKismetMathLibrary::ComposeRotators(GetControlRotation(), UpDirectionToWorldRotation.Rotator());
		if (Params.bReturnRotationInSpace2D) AnsRotationDS.SetComponentForAxis(EAxis::Y, 0.f);
		AnsRotationDS.Normalize();
		OutRotation = UKismetMathLibrary::ComposeRotators(AnsRotationDS, WorldToUpDirectionRotation.Rotator());
		
		return true;
	}
	
	return false;
}

