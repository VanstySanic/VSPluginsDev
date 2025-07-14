// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_FallingMovement.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSMathLibrary.h"
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

void UVSChrMovFeature_FallingMovement::LaunchCharacter(const FVector& NewVelocity, TEnumAsByte<EVSVectorAxes::Type> RelativeAxesToOverride, bool bReplicated)
{
	if (RelativeAxesToOverride == EVSVectorAxes::None) return;
	if (bReplicated && GetIsReplicated() && UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()))
	{
		LaunchCharacter_Server(NewVelocity, RelativeAxesToOverride);
	}
	else
	{
		LaunchCharacterInternal(NewVelocity, RelativeAxesToOverride);
	}
}

void UVSChrMovFeature_FallingMovement::LaunchCharacterInternal(const FVector& NewVelocity, TEnumAsByte<EVSVectorAxes::Type> RelativeAxesToOverride)
{
	const FVector& AxesedNewVelocity = UVSMathLibrary::VectorApplyAxes(GetVelocity(), NewVelocity, RelativeAxesToOverride, GetCharacter()->GetActorRotation());
	GetCharacter()->LaunchCharacter(AxesedNewVelocity, true, true);
}

void UVSChrMovFeature_FallingMovement::LaunchCharacter_Server_Implementation(const FVector& NewVelocity, EVSVectorAxes::Type RelativeAxesToOverride)
{
	LaunchCharacter_Multicast(NewVelocity, RelativeAxesToOverride);
}

void UVSChrMovFeature_FallingMovement::LaunchCharacter_Multicast_Implementation(const FVector& NewVelocity, EVSVectorAxes::Type RelativeAxesToOverride)
{
	LaunchCharacterInternal(NewVelocity, RelativeAxesToOverride);
}
