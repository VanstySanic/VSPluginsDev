// Copyright VanstySanic. All Rights Reserved.

#include "VSCharacterMovementUtils.h"
#include "VSCharacterMovementInterface.h"
#include "Components/CapsuleComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"

UVSCharacterMovementUtils::UVSCharacterMovementUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSCharacterMovementUtils::EvaluateCharacterMovementOrientation(const ACharacter* Character, FRotator& OutRotation, const FVSOrientationEvaluateParams& Params)
{
	if (!Character || !Character->GetClass()->ImplementsInterface(UVSCharacterMovementInterface::StaticClass())) return false;
	
	for (UVSChrMovFeature_OrientationEvaluator* Evaluator : IVSCharacterMovementInterface::Execute_GetMovementAgentFeature(Character)->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
	{
		if (Evaluator && Evaluator->EvaluateOrientation(OutRotation, Params)) return true;
	}
	
	UVSChrMovFeature_OrientationEvaluator::StaticClass()->GetDefaultObject<UVSChrMovFeature_OrientationEvaluator>()->EvaluateOrientation(OutRotation, Params);
	return false;
}

void UVSCharacterMovementUtils::SetCharacterMovementScale(ACharacter* Character, const FVector& NewScale)
{
	if (!Character) return;
	if (NewScale.X <= 0.f || NewScale.Y <= 0.f || NewScale.Z <= 0.f) return;

	const FVector& PrevScale = Character->GetActorScale3D();
	ApplyCharacterMovementScaleDelta(Character, NewScale, PrevScale);
}

void UVSCharacterMovementUtils::ApplyCharacterMovementScaleDelta(ACharacter* Character, const FVector& NewScale, const FVector& PrevScale)
{
	if (!Character) return;
	if (NewScale.X <= 0.f || NewScale.Y <= 0.f || NewScale.Z <= 0.f) return;

	Character->SetActorScale3D(NewScale);

	const float ScaleDeltaXY = NewScale.X / PrevScale.X;
	const float ScaleDeltaZ = NewScale.Z / PrevScale.X;
	const float ScaleDeltaG = NewScale.Size() / PrevScale.Size();
	
	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	CharacterMovement->MaxAcceleration *= ScaleDeltaXY;
	CharacterMovement->MaxDepenetrationWithGeometry *= ScaleDeltaG;
	CharacterMovement->MaxDepenetrationWithGeometryAsProxy *= ScaleDeltaG;
	CharacterMovement->MaxDepenetrationWithPawn *= ScaleDeltaG;
	CharacterMovement->MaxDepenetrationWithPawnAsProxy *= ScaleDeltaG;
	CharacterMovement->MaxStepHeight *= ScaleDeltaZ;
	CharacterMovement->MaxWalkSpeed *= ScaleDeltaXY;
	CharacterMovement->MaxWalkSpeedCrouched *= ScaleDeltaXY;
	CharacterMovement->MinAnalogWalkSpeed *= ScaleDeltaXY;
	CharacterMovement->BrakingDecelerationWalking *= ScaleDeltaXY;
	CharacterMovement->PerchRadiusThreshold *= ScaleDeltaG;
	CharacterMovement->PerchAdditionalHeight *= ScaleDeltaXY;
	CharacterMovement->LedgeCheckThreshold *= ScaleDeltaXY;
	CharacterMovement->JumpZVelocity *= ScaleDeltaZ;
	CharacterMovement->BrakingDecelerationFalling *= ScaleDeltaZ;
	CharacterMovement->StayBasedInAirHeight *= ScaleDeltaZ;
	CharacterMovement->NetworkMaxSmoothUpdateDistance *= ScaleDeltaG;
	CharacterMovement->NetworkNoSmoothUpdateDistance *= ScaleDeltaG;
	CharacterMovement->NetworkLargeClientCorrectionDistance *= ScaleDeltaG;
	CharacterMovement->MaxSwimSpeed *= ScaleDeltaZ;
	CharacterMovement->BrakingDecelerationSwimming *= ScaleDeltaG;
	CharacterMovement->MaxOutOfWaterStepHeight *= ScaleDeltaZ;
	CharacterMovement->OutofWaterZ *= ScaleDeltaZ;
	CharacterMovement->MaxFlySpeed *= ScaleDeltaG;
	CharacterMovement->MaxCustomMovementSpeed *= ScaleDeltaG;
	CharacterMovement->AvoidanceConsiderationRadius *= ScaleDeltaG;
	CharacterMovement->SetFixedBrakingDistance(CharacterMovement->GetNavMovementProperties()->FixedPathBrakingDistance * ScaleDeltaG);
	CharacterMovement->AvoidanceConsiderationRadius *= ScaleDeltaG;
	
	CharacterMovement->GravityScale *= ScaleDeltaZ;
}
