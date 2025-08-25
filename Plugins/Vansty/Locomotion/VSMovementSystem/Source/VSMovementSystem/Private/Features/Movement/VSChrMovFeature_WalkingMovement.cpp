// Copyright VanstySanic. All Rights Reserved.

#include "Features/Movement/VSChrMovFeature_WalkingMovement.h"
#include "VSCharacterMovementUtils.h"
#include "VSChrMovCapsuleComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Components/CapsuleComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
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

	MovementData.Gaits = DefaultGaits;

	SetStance(DefaultStance);
	MovementData.CurrentMovementBaseSettings = MovementData.Stance == EVSStance::Crouching ? DefaultCrouchedMovementBaseSettings : DefaultMovementBaseSettings;
}

void UVSChrMovFeature_WalkingMovement::EndPlay_Implementation()
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(MovementData.PrevStance, 0);
	GameplayTagController->SetTagCount(GetGait(MovementData.PrevStance), 0);
	GameplayTagController->NotifyTagsUpdated();

	Super::EndPlay_Implementation();
}

void UVSChrMovFeature_WalkingMovement::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	ListenToCrouchState();
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
		else if (!IsWalkingMode() && IsPrevWalkingMode())
		{
			if (GetStance() == EVSStance::Crouching)
			{
				GetCharacterMovement()->UnCrouch();
			}
			else if (!FMath::IsNearlyZero(MovementData.CapsuleHalfHeightOffsetUSCZ, 0.01f))
			{
				GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(GetMovementCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - MovementData.CapsuleHalfHeightOffsetUSCZ);
			}
			MovementData.CapsuleHalfHeightOffsetUSCZ = 0.f;
		}
	}
	else if (TagEvent == EVSMovementEvent::StateChange_Stance)
	{
		if (IsWalkingMode())
		{
			RefreshHalfHeight();
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

void UVSChrMovFeature_WalkingMovement::SetStance(const FGameplayTag& InStance, bool bReplicated)
{
	if (bReplicated && GetIsReplicated() && UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()))
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

void UVSChrMovFeature_WalkingMovement::SetGait(const FGameplayTag& InGait, const FGameplayTag& InStance, bool bReplicated)
{
	if (bReplicated && GetIsReplicated() && UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()))
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
	if (MovementData.Stance == EVSStance::Crouching && (!bIsOriginalCrouching && !GetCharacterMovement()->bWantsToCrouch))
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
	else if (MovementData.Stance != EVSStance::Crouching && (bIsOriginalCrouching && GetCharacterMovement()->bWantsToCrouch))
	{
		SetStanceInternal(EVSStance::Crouching);
	}
}

void UVSChrMovFeature_WalkingMovement::RefreshHalfHeight()
{
	if (!GetCharacter() || !IsWalkingMode()) return;

	const UCapsuleComponent* DefaultCapsule =  GetCharacter()->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent();

	const float LastUnscaledHalfHeight = GetCharacter()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float NewUnscaledHalfHeight = StancedHalfHeights.Contains(MovementData.Stance) ? StancedHalfHeights.FindRef(MovementData.Stance)
		: (MovementData.Stance == EVSStance::Crouching ? GetCharacterMovement()->GetCrouchedHalfHeight() : DefaultCapsule->GetUnscaledCapsuleHalfHeight());
	
	/** The crouching state is handled by the character movement. */
	if (GetStance() == EVSStance::Crouching || (GetStance() == EVSStance::Standing && GetPrevStance() == EVSStance::Crouching))
	{
		GetMovementCapsuleComponent()->SetCapsuleCenterOffsetZ(0.f);
		GetCharacter()->GetCapsuleComponent()->SetCapsuleHalfHeight(NewUnscaledHalfHeight);
		MovementData.CapsuleHalfHeightOffsetUSCZ = NewUnscaledHalfHeight - DefaultCapsule->GetUnscaledCapsuleHalfHeight();
		return;
	}
	
	const float DeltaUnscaledHalfHeight = NewUnscaledHalfHeight - LastUnscaledHalfHeight;
	if (!FMath::IsNearlyZero(DeltaUnscaledHalfHeight, 0.01f))
	{
		GetMovementCapsuleComponent()->SetCapsuleHalfHeightAndKeepRoot(NewUnscaledHalfHeight);
		MovementData.CapsuleHalfHeightOffsetUSCZ += DeltaUnscaledHalfHeight;
	}
}

void UVSChrMovFeature_WalkingMovement::ApplyMovementBaseSettings(const FVSMovementBaseSettings& Settings)
{
	if (!IsWalkingMode()) return;
	
	GetCharacterMovement()->MaxWalkSpeed = Settings.MaxSpeed * GetScale3D().X;
	GetCharacterMovement()->MaxWalkSpeedCrouched = Settings.MaxSpeed * GetScale3D().X;
		
	GetCharacterMovement()->MaxAcceleration = Settings.MaxAcceleration * GetScale3D().X;
	GetCharacterMovement()->BrakingDecelerationWalking = Settings.BrakingDeceleration * GetScale3D().X;
		
	GetCharacterMovement()->BrakingFriction = Settings.BrakingFriction * GetScale3D().X;
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
		MovementData.CurrentMovementBaseSettings = MovementData.Stance == EVSStance::Crouching ? DefaultCrouchedMovementBaseSettings : DefaultMovementBaseSettings;
		for (auto& Settings : QueriedMovementBaseSettings)
		{
			if (Settings.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentMovementBaseSettings = Settings.Key;
				break;
			}
		}
	}
	ApplyMovementBaseSettings(MovementData.CurrentMovementBaseSettings);
}

void UVSChrMovFeature_WalkingMovement::SetStanceInternal(const FGameplayTag& InStance)
{
	if (!GetCharacter()) return;

	/* Special judges for crouching. Process some crouch logic here. Sync the stance and crouch state. */
	const bool bIsOriginalCrouching = GetCharacterMovement()->IsCrouching();
	if (InStance == EVSStance::Crouching && (!bIsOriginalCrouching && !GetCharacterMovement()->bWantsToCrouch))
	{
		GetCharacter()->Crouch();
		return;
	}
	if (InStance != EVSStance::Crouching && (bIsOriginalCrouching && GetCharacterMovement()->bWantsToCrouch))
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
