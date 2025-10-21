// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeatureAgent.h"
#include "VSCharacterMovementUtils.h"
#include "VSChrMovCapsuleComponent.h"
#include "VSMovementSystemSettings.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "VSPrivablic.h"
#include "Net/UnrealNetwork.h"
#include "Types/VSCharacterMovementTags.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCharacterMovementComponent, Acceleration, FVector);

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
	Super::Super::Initialize_Implementation();
	
	ChrMovFeatureAgentPrivate = this;

	CharacterPrivate = Cast<ACharacter>(UVSGameplayLibrary::GetPawnFromObject(this));
	check(CharacterPrivate.IsValid());
	
	CharacterMovementComponentPrivate = CharacterPrivate->GetCharacterMovement();
	check(CharacterMovementComponentPrivate.IsValid() );

	GameplayTagControllerPrivate = UVSActorLibrary::GetGameplayTagControllerFromActor(CharacterPrivate.Get());
	check(GameplayTagControllerPrivate.IsValid());
	
	MovementCapsuleComponentPrivate = Cast<UVSChrMovCapsuleComponent>(GetCharacter()->GetCapsuleComponent());
	check(MovementCapsuleComponentPrivate.IsValid());
	
	CharacterPrivate->MovementModeChangedDelegate.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementModeChanged);
	
	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagEventNotified);
}

void UVSCharacterMovementFeatureAgent::Uninitialize_Implementation()
{
	if (CharacterPrivate.IsValid())
	{
		CharacterPrivate->MovementModeChangedDelegate.RemoveDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementModeChanged);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSCharacterMovementFeatureAgent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (CharacterPrivate->HasAuthority()) { ReplicatedControlRotation = CharacterPrivate->GetControlRotation(); }
	
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(GetMovementMode(), 1);

	if (IConsoleVariable* ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*FString("p.NetEnableMoveCombining", false)))
	{
		ConsoleVariable->SetWithCurrentPriority(ConsoleVariable->GetInt());
		ConsoleVariable->Set(ConsoleVariable->GetInt(), ECVF_SetByCode);
	}

	UVSCharacterMovementUtils::ApplyCharacterMovementScaleDelta(GetCharacter(), GetCharacter()->GetActorScale3D(), FVector(1));
	GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(GetMovementCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
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
	
	if (GetOwnerActor()->HasAuthority())
	{
		ReplicatedControlRotation = CharacterPrivate->GetControlRotation();
		ReplicatedMovementInput = GetCharacterMovement()->GetCurrentAcceleration();
	}
	else if (!UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()))
	{
		if (GetCharacterMovement())
		{
			VS_PRIVABLIC_MEMBER(GetCharacterMovement(), UCharacterMovementComponent, Acceleration) = ReplicatedMovementInput;
		}
	}

	if ((IsMoving2D() && !MovementData.bCachedIsMoving2D) || (!IsMoving2D() && MovementData.bCachedIsMoving2D))
	{
		GameplayTagController->SetTagCount(EVSMovementState::IsMoving2D, IsMoving2D() ? 1 : 0);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_IsMoving2D);
	}
	if ((HasAcceleration2D() && !MovementData.bCachedHasMovementInput2D) || (!HasAcceleration2D() && MovementData.bCachedHasMovementInput2D))
	{
		GameplayTagController->SetTagCount(EVSMovementState::HasAcceleration2D, HasAcceleration2D() ? 1 : 0);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_HasMovementInput2D);
	}
	MovementData.bCachedIsMoving2D = IsMoving2D();
	MovementData.bCachedHasMovementInput2D = HasAcceleration2D();

	CheckMovingAgainstWall2D();
}

void UVSCharacterMovementFeatureAgent::OnMovementTagsUpdated_Implementation()
{
	Super::OnMovementTagsUpdated_Implementation();
	
	const FGameplayTagContainer& GameplayTags = GetGameplayTagController()->GetGameplayTags();
	
	if (GetCharacter()->HasAuthority())
	{
		if (NetworkIgnoreClientCorrectionTagQuery.Matches(GameplayTags))
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
		if (NetworkDisableMoveCombiningTagQuery.Matches(GameplayTags))
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
	const bool bPrevMovingAgainstWall2D = MovementData.bIsMovingAgainstWall2D;
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

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	if ((!bPrevMovingAgainstWall2D && MovementData.bIsMovingAgainstWall2D) || (bPrevMovingAgainstWall2D && !MovementData.bIsMovingAgainstWall2D))
	{
		GameplayTagController->SetTagCount(EVSMovementState::IsMovingAgainstWall2D, MovementData.bIsMovingAgainstWall2D ? 1 : 0);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_IsMovingAgainstWall2D);
	}
}

void UVSCharacterMovementFeatureAgent::OnCharacterMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (!GetCharacter()) return;
	
	const FGameplayTag& PrevMovementModeTag = UVSMovementSystemSettings::GetTagFromMovementMode(PrevMovementMode, PreviousCustomMode);
	MovementData.PrevMovementMode = PrevMovementModeTag;

	const ACharacter* DefaultCharacter = GetCharacter()->GetClass()->GetDefaultObject<ACharacter>();
	
	/** Reset movement base settings to default. */
	const UCharacterMovementComponent* DefaultCharacterMove = DefaultCharacter->GetCharacterMovement();
	GetCharacterMovement()->MaxWalkSpeed = DefaultCharacterMove->MaxWalkSpeed * GetScale3D().X;
	GetCharacterMovement()->MaxAcceleration = DefaultCharacterMove->MaxAcceleration* GetScale3D().X;
	GetCharacterMovement()->BrakingFriction = DefaultCharacterMove->BrakingFriction* GetScale3D().X;
	GetCharacterMovement()->bUseSeparateBrakingFriction = DefaultCharacterMove->bUseSeparateBrakingFriction* GetScale3D().X;
	GetCharacterMovement()->BrakingFrictionFactor = DefaultCharacterMove->BrakingFrictionFactor* GetScale3D().X;
	
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(PrevMovementModeTag, 0);
	GameplayTagController->SetTagCount(GetMovementMode(), 1);
	GameplayTagController->NotifyTagsUpdated();
	GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_MovementMode);
	
	OnMovementModeChanged.Broadcast(GetMovementMode(), PrevMovementModeTag);
}