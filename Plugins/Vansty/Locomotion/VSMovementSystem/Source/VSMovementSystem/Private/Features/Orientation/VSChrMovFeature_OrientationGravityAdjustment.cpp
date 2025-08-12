// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_OrientationGravityAdjustment.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSMathLibrary.h"
#include "Types/VSCharacterMovementTags.h"

UVSChrMovFeature_OrientationGravityAdjustment::UVSChrMovFeature_OrientationGravityAdjustment()
{

}

void UVSChrMovFeature_OrientationGravityAdjustment::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	MovementData.bMatchesTagQuery = MovementStateQuerySettings.Matches(GetGameplayTagController()->GetGameplayTags(), EVSMovementEvent::StateChange_MovementMode);
}

bool UVSChrMovFeature_OrientationGravityAdjustment::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && MovementData.bMatchesTagQuery;
}

void UVSChrMovFeature_OrientationGravityAdjustment::UpdateMovement_Implementation(float DeltaTime)
{
	const FRotator& DeltaRotation = FQuat::FindBetweenNormals(-GetUpDirection(), (bIsAntiGravity ? -1.f : 1.f) * GetGravityDirection()).Rotator();
	GetCharacter()->AddActorWorldRotation(DeltaRotation);
}

void UVSChrMovFeature_OrientationGravityAdjustment::OnMovementTagEventNotified_Implementation( const FGameplayTag& TagEvent)
{
	MovementData.bMatchesTagQuery = MovementStateQuerySettings.Matches(GetGameplayTagController()->GetGameplayTags(), TagEvent);
}
