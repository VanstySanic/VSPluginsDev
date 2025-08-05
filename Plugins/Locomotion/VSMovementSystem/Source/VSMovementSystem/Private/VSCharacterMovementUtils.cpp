// Copyright VanstySanic. All Rights Reserved.

#include "VSCharacterMovementUtils.h"
#include "VSCharacterMovementInterface.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"

UVSCharacterMovementUtils::UVSCharacterMovementUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSCharacterMovementFeatureAgent* UVSCharacterMovementUtils::GetCharacterMovementFeatureAgentFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	UVSCharacterMovementFeatureAgent* FeatureAgent = nullptr;
	if (!FeatureAgent)
	{
		if (Actor->GetClass()->ImplementsInterface(UVSCharacterMovementInterface::StaticClass()))
		{
			FeatureAgent = IVSCharacterMovementInterface::Execute_GetMovementAgentFeature(Actor);
		}
	}
	if (!FeatureAgent)
	{
		FeatureAgent = UVSActorLibrary::FindFeatureByClassFromActor<UVSCharacterMovementFeatureAgent>(Actor);
	}
	return FeatureAgent;
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
	CharacterMovement->AvoidanceConsiderationRadius *= ScaleDeltaG;
	CharacterMovement->SetFixedBrakingDistance(CharacterMovement->GetNavMovementProperties()->FixedPathBrakingDistance * ScaleDeltaG);

	CharacterMovement->GravityScale *= ScaleDeltaZ;
}
