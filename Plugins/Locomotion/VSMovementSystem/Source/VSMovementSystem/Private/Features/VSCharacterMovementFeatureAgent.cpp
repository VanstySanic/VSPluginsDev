// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeatureAgent.h"

#include "VSCharacterMovementUtils.h"
#include "VSChrMovCapsuleComponent.h"
#include "VSMovementSystemSettings.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "VSPrivablic.h"
#include "Net/UnrealNetwork.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCharacterMovementComponent, Acceleration, FVector);
VS_DECLARE_PRIVABLIC_MEMBER(APawn, LastControlInputVector, FVector);

UVSCharacterMovementFeatureAgent::UVSCharacterMovementFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCharacterMovementFeatureAgent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UVSCharacterMovementFeatureAgent, ReplicatedControlRotation, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UVSCharacterMovementFeatureAgent, ReplicatedMovementInput, COND_SimulatedOnly);
}

void UVSCharacterMovementFeatureAgent::Initialize_Implementation()
{
	ChrMovFeatureAgentPrivate = Cast<UVSCharacterMovementFeatureAgent>(this);

	CharacterPrivate = GetTypedOuter<ACharacter>();
	check(CharacterPrivate.IsValid() && CharacterPrivate->Implements<UVSGameplayTagControllerInterface>());
	
	CharacterMovementComponentPrivate = CharacterPrivate->GetCharacterMovement();
	check(CharacterMovementComponentPrivate.IsValid() );
	
	GameplayTagControllerPrivate = IVSGameplayTagControllerInterface::Execute_GetGameplayTagController(CharacterPrivate.Get());
	check(GameplayTagControllerPrivate.IsValid());
	
	CharacterPrivate->MovementModeChangedDelegate.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementChanged);
	if (CharacterPrivate->HasAuthority()) { ReplicatedControlRotation = CharacterPrivate->GetControlRotation(); }

	MovementCapsuleComponentPrivate = Cast<UVSChrMovCapsuleComponent>(GetCharacter()->GetCapsuleComponent());
	check(MovementCapsuleComponentPrivate.IsValid());
	
	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagEventNotified);
}

void UVSCharacterMovementFeatureAgent::Uninitialize_Implementation()
{
	if (CharacterPrivate.IsValid())
	{
		CharacterPrivate->MovementModeChangedDelegate.RemoveDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementChanged);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSCharacterMovementFeatureAgent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(GetMovementMode(), 1);

	if (IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*FString("p.NetEnableMoveCombining", false)))
	{
		ConsoleVariable->SetWithCurrentPriority(ConsoleVariable->GetInt());
		ConsoleVariable->Set(ConsoleVariable->GetInt(), ECVF_SetByCode);
	}
}

void UVSCharacterMovementFeatureAgent::EndPlay_Implementation()
{
	Super::EndPlay_Implementation();
}

void UVSCharacterMovementFeatureAgent::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);
	
	MovementData.RealAcceleration = (GetVelocity() - MovementData.CachedVelocity) / DeltaTime;
	MovementData.CachedVelocity = GetVelocity();

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	if ((IsMoving2D() && !MovementData.bCachedIsMoving2D) || (!IsMoving2D() && MovementData.bCachedIsMoving2D))
	{
		GameplayTagController->SetTagCount(EVSMovementState::IsMoving2D, IsMoving2D() ? 1 : 0);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_IsMoving2D);
	}
	if ((HasMovementInput2D() && !MovementData.bCachedHasMovementInput2D) || (!HasMovementInput2D() && MovementData.bCachedHasMovementInput2D))
	{
		GameplayTagController->SetTagCount(EVSMovementState::HasMovementInput2D, HasMovementInput2D() ? 1 : 0);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_HasMovementInput2D);
	}
	MovementData.bCachedIsMoving2D = IsMoving2D();
	MovementData.bCachedHasMovementInput2D = HasMovementInput2D();
	
	CheckMovingAgainstWall2D();

	if (GetOwnerActor()->HasAuthority())
	{
		ReplicatedControlRotation = CharacterPrivate->GetControlRotation();
		ReplicatedMovementInput = GetCharacterMovement()->GetCurrentAcceleration();
	}
}

void UVSCharacterMovementFeatureAgent::OnMovementTagsUpdated_Implementation()
{
	Super::OnMovementTagsUpdated_Implementation();
	
	const FGameplayTagContainer& GameplayTags = GetGameplayTagController()->GetGameplayTags();
	if (GetCharacter()->HasAuthority())
	{
		if (NetworkIgnoreClientCorrectionQuery.Matches(GameplayTags))
		{
			GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;
		}
		else
		{
			const ACharacter* DefaultCharacter = GetCharacter()->GetClass()->GetDefaultObject<ACharacter>();
			const UCharacterMovementComponent* DefaultCharacterMove = DefaultCharacter->GetCharacterMovement();
			GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = DefaultCharacterMove->bIgnoreClientMovementErrorChecksAndCorrection;
		}
	}
	if (GetCharacter()->IsLocallyControlled())
	{
		if (NetworkDisableMoveCombiningQuery.Matches(GameplayTags))
		{
			if (IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*FString("p.NetEnableMoveCombining"), false))
			{
				ConsoleVariable->SetWithCurrentPriority(0);
			}	
		}
		else
		{
			if (IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*FString("p.NetEnableMoveCombining"), false))
			{
				ConsoleVariable->SetWithCurrentPriority(FCString::Atoi(*ConsoleVariable->GetDefaultValue()));
			}	
		}
	}

}

void UVSCharacterMovementFeatureAgent::CheckMovingAgainstWall2D()
{
	/** Check whether the character is moving against the wall in 2D direction. */
	MovementData.bIsMovingAgainstWall2D = false;
	if (IsMoving2D())
	{
		const FCollisionShape& TraceShape = GetCharacter()->GetCapsuleComponent()->GetCollisionShape();
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(GetCharacter());
		FHitResult HitResult;
		
		const FVector& TraceDirection = (GetMovementInput2D().Size() ? GetMovementInput2D() : GetVelocity2D()).GetSafeNormal();
		GetWorld()->SweepSingleByChannel(
				HitResult,
				GetCharacter()->GetActorLocation(),
				GetCharacter()->GetActorLocation() + TraceDirection,
				GetCharacter()->GetActorQuat(),
				ECC_Pawn,
				TraceShape,
				TraceParams);

		MovementData.bIsMovingAgainstWall2D = HitResult.IsValidBlockingHit();
	}
}

void UVSCharacterMovementFeatureAgent::OnCharacterMovementChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (!GetCharacter()) return;
	
	const FGameplayTag& PrevMovementModeTag = UVSMovementSystemSettings::GetTagFromMovementMode(PrevMovementMode, PreviousCustomMode);
	MovementData.PrevMovementMode = PrevMovementModeTag;

	/** Reset capsule halfheight. */
	const ACharacter* DefaultCharacter = GetCharacter()->GetClass()->GetDefaultObject<ACharacter>();
	// GetCharacter()->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	
	/** Reset movement base settings to default. */
	const UCharacterMovementComponent* DefaultCharacterMove = DefaultCharacter->GetCharacterMovement();
	GetCharacterMovement()->MaxWalkSpeed = DefaultCharacterMove->MaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = DefaultCharacterMove->MaxAcceleration;
	// GetCharacterMovement()->BrakingDecelerationWalking = DefaultCharacterMove->BrakingDecelerationWalking;
	GetCharacterMovement()->BrakingFriction = DefaultCharacterMove->BrakingFriction;
	GetCharacterMovement()->bUseSeparateBrakingFriction = DefaultCharacterMove->bUseSeparateBrakingFriction;
	GetCharacterMovement()->BrakingFrictionFactor = DefaultCharacterMove->BrakingFrictionFactor;
	
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(PrevMovementModeTag, 0);
	GameplayTagController->SetTagCount(GetMovementMode(), 1);
	GameplayTagController->NotifyTagsUpdated();
	GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_MovementMode);
	
	OnMovementModeChanged.Broadcast(GetMovementMode(), PrevMovementModeTag);
}