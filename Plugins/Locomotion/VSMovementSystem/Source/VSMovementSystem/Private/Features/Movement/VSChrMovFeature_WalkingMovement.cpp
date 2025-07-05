// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_WalkingMovement.h"

#include "VSCharacterMovementUtils.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Types/VSCharacterMovementTags.h"

UVSChrMovFeature_WalkingMovement::UVSChrMovFeature_WalkingMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultStance = EVSStance::Standing;
	DefaultGaits = {
		{EVSStance::Standing, EVSGait::Running},
		{EVSStance::Crouching, EVSGait::Walking}
	};

	DefaultUncrouchedStance = EVSStance::Standing;
	StancedHalfHeights.Emplace(EVSStance::Crouching, 60.f);

	DefaultMovementBaseSettings.MaxSpeed = 600.f;
	DefaultCrouchedMovementBaseSettings.MaxSpeed = 240.f;
}


void UVSChrMovFeature_WalkingMovement::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UVSChrMovFeature_WalkingMovement, ReplicatedStance, SharedParams);
}

void UVSChrMovFeature_WalkingMovement::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	MovementData.Stance = DefaultStance;
	MovementData.Gaits = DefaultGaits;
	MovementData.CurrentMovementBaseSettings = IsCrouching() ? DefaultCrouchedMovementBaseSettings : DefaultMovementBaseSettings;
	ReplicatedStance = DefaultStance;
	GameplayTagController->SetTagCount(DefaultStance, 1);
	GameplayTagController->SetTagCount(GetGait(DefaultStance), 1);
	RefreshHalfHeight();
	ApplyMovementBaseSettings(MovementData.CurrentMovementBaseSettings);
	RefreshMovementBaseSettings(EVSMovementEvent::StateChange_Stance);
}

void UVSChrMovFeature_WalkingMovement::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	ListenToCrouchState();
}

bool UVSChrMovFeature_WalkingMovement::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && IsWalkingMode();
}

void UVSChrMovFeature_WalkingMovement::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		if (IsWalkingMode() && !IsPrevWalkingMode())
		{
			RefreshHalfHeight();
			ApplyMovementBaseSettings(MovementData.CurrentMovementBaseSettings);
		}
		else if (!IsWalkingMode())
		{
			GetCharacter()->GetMesh()->AddRelativeLocation(FVector(0.0, 0.0, -MovementData.RelativeOffsetToMeshZ));
			MovementData.RelativeOffsetToMeshZ = 0.f;
		}
	}
	if (IsWalkingMode())
	{
		RefreshMovementBaseSettings(TagEvent);
	}
}

bool UVSChrMovFeature_WalkingMovement::IsWalkingMode() const
{
	return GetMovementMode() == EVSMovementMode::Walking || GetMovementMode() == EVSMovementMode::NavWalking;
}

bool UVSChrMovFeature_WalkingMovement::IsPrevWalkingMode() const
{
	return GetPrevMovementMode() == EVSMovementMode::Walking || GetPrevMovementMode() == EVSMovementMode::NavWalking;
}

bool UVSChrMovFeature_WalkingMovement::IsCrouching() const
{
	return MovementData.Stance == EVSStance::Crouching;
}

FGameplayTag UVSChrMovFeature_WalkingMovement::GetGait(const FGameplayTag& InStance) const
{
	const FGameplayTag& StanceToFind = InStance == FGameplayTag::EmptyTag ? GetStance() : InStance;
	if (const FGameplayTag* GaitPtr = MovementData.Gaits.Find(StanceToFind))
	{
		return *GaitPtr;
	}

	return FGameplayTag::EmptyTag;
}

FGameplayTag UVSChrMovFeature_WalkingMovement::GetPrevGait(const FGameplayTag& InStance) const
{
	const FGameplayTag& StanceToFind = InStance == FGameplayTag::EmptyTag ? GetStance() : InStance;
	if (const FGameplayTag* GaitPtr = MovementData.PrevGaits.Find(StanceToFind))
	{
		return *GaitPtr;
	}

	return FGameplayTag::EmptyTag;
}

void UVSChrMovFeature_WalkingMovement::SetStance(const FGameplayTag& InStance)
{
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetStance_Server(InStance);
	}
	else
	{
		SetStanceInternal(InStance);
	}
}

void UVSChrMovFeature_WalkingMovement::SetStance_Server_Implementation(const FGameplayTag& InStance)
{
	ReplicatedStance = InStance;
	MARK_PROPERTY_DIRTY_FROM_NAME(UVSChrMovFeature_WalkingMovement, ReplicatedStance, this);
	if (GetOwnerActor() && GetOwnerActor()->HasAuthority())
	{
		OnRep_ReplicatedStance();
	}
}

void UVSChrMovFeature_WalkingMovement::SetGait(const FGameplayTag& InGait, const FGameplayTag& InStance)
{
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetGait_Server(InGait, InStance);
	}
	else
	{
		SetGaitInternal(InGait, InStance);
	}
}

void UVSChrMovFeature_WalkingMovement::ListenToCrouchState()
{
	/* Listen to the crouch state here, so user needn't do it in the character. Sync the stance and crouch state. */
	const bool bIsOriginalCrouching = GetCharacterMovement()->IsCrouching();
	if (MovementData.Stance == EVSStance::Crouching && (!bIsOriginalCrouching || !GetCharacterMovement()->bWantsToCrouch))
	{
		if (MovementData.DesiredUncrouchedStance != FGameplayTag::EmptyTag)
		{
			SetStanceInternal(MovementData.DesiredUncrouchedStance);
			MovementData.DesiredUncrouchedStance = FGameplayTag::EmptyTag;
		}
		else
		{
			SetStanceInternal(DefaultUncrouchedStance);
		}
	}
	else if (MovementData.Stance != EVSStance::Crouching && (bIsOriginalCrouching || GetCharacterMovement()->bWantsToCrouch))
	{
		SetStanceInternal(EVSStance::Crouching);
	}
}

void UVSChrMovFeature_WalkingMovement::RefreshHalfHeight()
{
	if (!GetCharacter() || !IsWalkingMode()) return;
	
	const float LastUnscaledHalfHeight = GetCharacter()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	
	const float NewUnscaledHalfHeight = StancedHalfHeights.Contains(MovementData.Stance) ? StancedHalfHeights.FindRef(MovementData.Stance)
		: (IsCrouching() ? GetCharacterMovement()->GetCrouchedHalfHeight() : GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());

	const float DeltaHalfHeight = NewUnscaledHalfHeight - LastUnscaledHalfHeight;
	
	if (!FMath::IsNearlyZero(DeltaHalfHeight))
	{
		/** Move the capsule to lock the root location. */
		GetCharacter()->GetCapsuleComponent()->SetCapsuleHalfHeight(NewUnscaledHalfHeight);
		GetCharacter()->GetCapsuleComponent()->AddLocalOffset(FVector(0.0, 0.0, DeltaHalfHeight));
		GetCharacter()->GetMesh()->AddRelativeLocation(FVector(0.0, 0.0, -DeltaHalfHeight));
		MovementData.RelativeOffsetToMeshZ -= DeltaHalfHeight;
	}
}

void UVSChrMovFeature_WalkingMovement::ApplyMovementBaseSettings(const FVSMovementBaseSettings& Settings)
{
	if (!IsWalkingMode()) return;
	
	GetCharacterMovement()->MaxWalkSpeed = Settings.MaxSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = Settings.MaxSpeed;
		
	GetCharacterMovement()->MaxAcceleration = Settings.MaxAcceleration;
	GetCharacterMovement()->BrakingDecelerationWalking = Settings.BrakingDeceleration;
		
	GetCharacterMovement()->BrakingFriction = Settings.BrakingFriction;
	GetCharacterMovement()->bUseSeparateBrakingFriction = Settings.bUseSeparateBrakingFriction;
	GetCharacterMovement()->BrakingFrictionFactor = Settings.BrakingFrictionFactor;
}

void UVSChrMovFeature_WalkingMovement::RefreshMovementBaseSettings(const FGameplayTag& TagEvent)
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	FGameplayTagContainer GameplayTags;
	GameplayTagController->GetOwnedGameplayTags(GameplayTags);

	if (RefreshMovementBaseSettingsQueries.Matches(GameplayTags, TagEvent))
	{
		MovementData.CurrentMovementBaseSettings = IsCrouching() ? DefaultCrouchedMovementBaseSettings : DefaultMovementBaseSettings;
		for (auto& Settings : QueriedMovementBaseSettings)
		{
			if (Settings.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentMovementBaseSettings = Settings.Key;
				break;
			}
		}
		ApplyMovementBaseSettings(MovementData.CurrentMovementBaseSettings);
	}
}

void UVSChrMovFeature_WalkingMovement::SetStanceInternal(const FGameplayTag& InStance)
{
	if (!GetCharacter()) return;

	/* Special judges for crouching. Process some crouch logic here. Sync the stance and crouch state. */
	const bool bIsOriginalCrouching = GetCharacterMovement()->IsCrouching();
	if (InStance == EVSStance::Crouching && (!bIsOriginalCrouching || !GetCharacterMovement()->bWantsToCrouch))
	{
		GetCharacter()->Crouch();
		return;
	}
	if (InStance != EVSStance::Crouching && (bIsOriginalCrouching || GetCharacterMovement()->bWantsToCrouch))
	{
		GetCharacter()->UnCrouch();
		MovementData.DesiredUncrouchedStance = InStance;
		return;
	}
	
	const FGameplayTag& CurrentStance = GetStance();
	if (CurrentStance == InStance) return;

	MovementData.PrevStance = CurrentStance;
	MovementData.Stance = InStance;

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(MovementData.PrevStance, 0);
	GameplayTagController->SetTagCount(MovementData.Stance, 1);
	GameplayTagController->SetTagCount(GetGait(MovementData.PrevStance), 0);
	GameplayTagController->SetTagCount(GetGait(MovementData.Stance), 1);
	GameplayTagController->NotifyTagsUpdated();
	GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_Stance);

	OnStanceChanged.Broadcast(MovementData.Stance, MovementData.PrevStance);
}

void UVSChrMovFeature_WalkingMovement::SetGaitInternal(const FGameplayTag& InGait, const FGameplayTag& InStance)
{
	const FGameplayTag& StanceToFind = InStance == FGameplayTag::EmptyTag ? GetStance() : InStance;
	const FGameplayTag& PrevGait = GetGait(InStance);
	if (PrevGait == InGait) return;

	MovementData.PrevGaits.Emplace(InStance, PrevGait);
	MovementData.Gaits.Emplace(StanceToFind, InGait);
	if (InStance == MovementData.Stance && InGait == GetGait())
	{
		UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
		GameplayTagController->SetTagCount(PrevGait, 0);
		GameplayTagController->SetTagCount(InGait, 1);
		GameplayTagController->NotifyTagsUpdated();
		GameplayTagController->NotifyTagEvent(EVSMovementEvent::StateChange_Gait);
		OnGaitChanged.Broadcast(InGait, PrevGait);
	}
}

void UVSChrMovFeature_WalkingMovement::SetGait_Server_Implementation(const FGameplayTag& InGait, const FGameplayTag& InStance)
{
	SetGait_Multicast(InGait, InStance);
}

void UVSChrMovFeature_WalkingMovement::SetGait_Multicast_Implementation(const FGameplayTag& InGait, const FGameplayTag& InStance)
{
	SetGaitInternal(InGait, InStance);
}

void UVSChrMovFeature_WalkingMovement::OnRep_ReplicatedStance()
{
	SetStanceInternal(ReplicatedStance);
}
