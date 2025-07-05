// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeatureAgent.h"
#include "VSMovementSystemSettings.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "Libraries/VSPrivablicLibrary.h"
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
	
	MovementData.RealAcceleration = (GetVelocity() - MovementData.CachedVelocity) / DeltaTime;
	MovementData.CachedVelocity = GetVelocity();
	
	CheckMovingAgainstWall2D();
	
	if (GetOwnerActor()->HasAuthority())
	{
		ReplicatedControlRotation = CharacterPrivate->GetControlRotation();
		ReplicatedMovementInput = GetCharacterMovement()->GetCurrentAcceleration();
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