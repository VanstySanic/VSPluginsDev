// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_FallingMovement.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Types/VSCharacterMovementTags.h"

UVSChrMovFeature_FallingMovement::UVSChrMovFeature_FallingMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSChrMovFeature_FallingMovement::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && GetMovementMode() == EVSMovementMode::Falling;
}

void UVSChrMovFeature_FallingMovement::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	if (MovementData.CachedJumpCount != GetCharacter()->JumpCurrentCount)
	{
		GetGameplayTagController()->NotifyTagEvent(EVSMovementEvent::Action_Jump);
		MovementData.CachedJumpCount = GetCharacter()->JumpCurrentCount;
	}
}

void UVSChrMovFeature_FallingMovement::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (GetMovementMode() == EVSMovementMode::Falling)
		{
			if (GetCharacter()->bPressedJump
				&& GetVelocityZ().Dot(GetCharacter()->GetActorUpVector()) > 0.9f
				&& FMath::IsNearlyEqual(GetSpeedZ(), GetCharacterMovement()->JumpZVelocity, GetCharacterMovement()->JumpZVelocity * 0.01f))
			{
				GetGameplayTagController()->NotifyTagEvent(EVSMovementEvent::Action_Jump);
				MovementData.CachedJumpCount = 1;
			}
		}
		else if (GetPrevMovementMode() == EVSMovementMode::Falling)
		{
			MovementData.CachedJumpCount = 0;
		}
	}
}
