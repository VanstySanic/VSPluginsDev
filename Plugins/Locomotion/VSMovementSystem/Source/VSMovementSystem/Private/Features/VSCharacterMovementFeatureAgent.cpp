// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeatureAgent.h"
#include "VSMovementSystemSettings.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"

UVSCharacterMovementFeatureAgent::UVSCharacterMovementFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCharacterMovementFeatureAgent::Initialize_Implementation()
{
	ChrMovFeatureAgentPrivate = Cast<UVSCharacterMovementFeatureAgent>(this);

	CharacterPrivate = GetTypedOuter<ACharacter>();
	check(CharacterPrivate.IsValid());
	
	CharacterMovementComponentPrivate = CharacterPrivate->GetCharacterMovement();
	check(CharacterMovementComponentPrivate.IsValid());
	
	if (CharacterPrivate->Implements<UVSGameplayTagControllerInterface>())
	{
		GameplayTagControllerPrivate = IVSGameplayTagControllerInterface::Execute_GetGameplayTagController_BP(CharacterPrivate.Get());
	}
	check(GameplayTagControllerPrivate.IsValid());
	
	CharacterPrivate->MovementModeChangedDelegate.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementChanged);

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
}

void UVSCharacterMovementFeatureAgent::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	if (DeltaTime < UE_KINDA_SMALL_NUMBER) return;

	MovementData.RealAcceleration = (GetVelocity() - CharacterMovementComponentPrivate->GetLastUpdateVelocity()) / DeltaTime;
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
	GetCharacter()->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	
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
