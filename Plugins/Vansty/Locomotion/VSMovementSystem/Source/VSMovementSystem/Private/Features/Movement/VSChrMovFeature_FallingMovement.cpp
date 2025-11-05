// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_FallingMovement.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSMathLibrary.h"
#include "Types/VSCharacterMovementTags.h"

UVSChrMovFeature_FallingMovement::UVSChrMovFeature_FallingMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
}

bool UVSChrMovFeature_FallingMovement::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && GetMovementMode() == EVSMovementMode::Falling;
}

void UVSChrMovFeature_FallingMovement::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	if (GetCharacter()->JumpCurrentCount > MovementData.CachedJumpCount)
	{
		GetGameplayTagController()->NotifyTagEvent(EVSMovementEvent::Action_Jump,
			FVSNetMethodExecutionPolicies(
			EVSNetAutonomousMethodExecPolicy::Client,
			EVSNetAuthorityMethodExecPolicy::ServerAndSimulated,
			EVSNetAuthorityMethodExecPolicy::None,
			false));
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
			if (GetCharacter()->bPressedJump && GetCharacter()->JumpCurrentCount > MovementData.CachedJumpCount)
			{
				GetGameplayTagController()->NotifyTagEvent(EVSMovementEvent::Action_Jump,
					FVSNetMethodExecutionPolicies(
					EVSNetAutonomousMethodExecPolicy::Client,
					EVSNetAuthorityMethodExecPolicy::ServerAndSimulated,
					EVSNetAuthorityMethodExecPolicy::None,
					false));
				MovementData.CachedJumpCount = GetCharacter()->JumpCurrentCount;
			}
		}
		else if (GetPrevMovementMode() == EVSMovementMode::Falling)
		{
			MovementData.CachedJumpCount = 0;
		}
	}
	/** TODO The jump count always late on simulated client. Need to repair this. */
	if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy && TagEvent == EVSMovementEvent::Action_Jump)
	{
		MovementData.CachedJumpCount++;
		GetCharacter()->JumpCurrentCount = MovementData.CachedJumpCount;
	}
}

void UVSChrMovFeature_FallingMovement::LaunchCharacter(const FVector& NewVelocity, TEnumAsByte<EVSVectorAxes::Type> RelativeAxesToOverride, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (RelativeAxesToOverride == EVSVectorAxes::None) return;
	
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			LaunchCharacterInternal(NewVelocity, RelativeAxesToOverride);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			LaunchCharacter_Server(NewVelocity, RelativeAxesToOverride, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_Authority)
	{
		LaunchCharacter_Server(NewVelocity, RelativeAxesToOverride, NetExecPolicies.ServerRPCPolicy);
	}
	else if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			LaunchCharacterInternal(NewVelocity, RelativeAxesToOverride);
		}
	}
}

void UVSChrMovFeature_FallingMovement::LaunchCharacterInternal(const FVector& NewVelocity, TEnumAsByte<EVSVectorAxes::Type> RelativeAxesToOverride)
{
	const FVector& AxesedNewVelocity = UVSMathLibrary::VectorApplyAxes(GetVelocity(), NewVelocity, RelativeAxesToOverride, GetCharacter()->GetActorRotation());
	GetCharacter()->LaunchCharacter(AxesedNewVelocity, true, true);
}

void UVSChrMovFeature_FallingMovement::LaunchCharacter_Server_Implementation(const FVector& NewVelocity, EVSVectorAxes::Type RelativeAxesToOverride, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		LaunchCharacterInternal(NewVelocity, RelativeAxesToOverride);
	}
	if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
	{
		LaunchCharacter_Multicast(NewVelocity, RelativeAxesToOverride, NetExecPolicy);
	}
}

void UVSChrMovFeature_FallingMovement::LaunchCharacter_Multicast_Implementation(const FVector& NewVelocity, EVSVectorAxes::Type RelativeAxesToOverride, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;

	/** Authority already handled. */
	if (GetCharacter()->HasAuthority()) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetCharacter()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		LaunchCharacterInternal(NewVelocity, RelativeAxesToOverride);
	}
}
