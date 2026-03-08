// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeature.h"
#include "VSCharacterMovementSystemUtils.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSObjectLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UVSCharacterMovementFeature::UVSCharacterMovementFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCharacterMovementFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	CharacterMovementFeatureAgentPrivate = GetCharacterMovementFeatureAgent();
	check(CharacterMovementFeatureAgentPrivate.IsValid());
}

void UVSCharacterMovementFeature::Uninitialize_Implementation()
{
	CharacterMovementFeatureAgentPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

FVector UVSCharacterMovementFeature::GetAngularVelocity_Implementation() const
{
	return CharacterMovementFeatureAgentPrivate.IsValid() ? CharacterMovementFeatureAgentPrivate->RealMovementAngularVelocity : FVector::ZeroVector;
}

ACharacter* UVSCharacterMovementFeature::GetCharacter() const
{
	return CharacterMovementFeatureAgentPrivate.IsValid() && CharacterMovementFeatureAgentPrivate->CharacterPrivate.IsValid()
		? CharacterMovementFeatureAgentPrivate->CharacterPrivate.Get()
		: Cast<ACharacter>(GetOwnerActor());
}

UCharacterMovementComponent* UVSCharacterMovementFeature::GetCharacterMovement() const
{
	return GetCharacter() ? GetCharacter()->GetCharacterMovement() : nullptr;
}

UVSCharacterMovementFeatureAgent* UVSCharacterMovementFeature::GetCharacterMovementFeatureAgent() const
{
	if (CharacterMovementFeatureAgentPrivate.IsValid()) return CharacterMovementFeatureAgentPrivate.Get();
	if (UVSCharacterMovementFeatureAgent* FeatureAgent = Cast<UVSCharacterMovementFeatureAgent>(const_cast<UVSCharacterMovementFeature*>(this))) return FeatureAgent;
	if (UVSCharacterMovementFeatureAgent* FeatureAgent = FindOwnerFeatureByClass<UVSCharacterMovementFeatureAgent>()) return FeatureAgent;
	return UVSObjectLibrary::FindFeatureByClassFromObject<UVSCharacterMovementFeatureAgent>(GetOwnerActor());
}

FVector UVSCharacterMovementFeature::GetAcceleration_Implementation() const
{
	return CharacterMovementFeatureAgentPrivate.IsValid() ? CharacterMovementFeatureAgentPrivate->RealMovementAcceleration : FVector::ZeroVector;
}

FVector UVSCharacterMovementFeature::GetMovementInput_Implementation() const
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		return CharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero() && !CharacterMovementComponent->GetPendingInputVector().IsNearlyZero()
			? CharacterMovementComponent->GetPendingInputVector()
			: CharacterMovementComponent->GetLastInputVector();
	}
	
	return Super::GetMovementInput_Implementation();
}

USceneComponent* UVSCharacterMovementFeature::GetUpdatedComponent_Implementation() const
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		return CharacterMovementComponent->UpdatedComponent;
	}
	
	return Super::GetUpdatedComponent_Implementation();
}

UPrimitiveComponent* UVSCharacterMovementFeature::GetMovementBase_Implementation() const
{
	return GetCharacterMovement() ? GetCharacterMovement()->GetMovementBase() : nullptr;
}

FName UVSCharacterMovementFeature::GetMovementBaseBoneName_Implementation() const
{
	return GetCharacter() ? GetCharacter()->GetBasedMovement().BoneName : NAME_None;
}

FGameplayTag UVSCharacterMovementFeature::GetMovementMode_Implementation() const
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		return UVSCharacterMovementSystemUtils::GetTaggedMovementMode(CharacterMovementComponent->MovementMode, CharacterMovementComponent->CustomMovementMode);
	}
	
	return Super::GetMovementMode_Implementation();
}


UVSCharacterMovementFeatureAgent::UVSCharacterMovementFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCharacterMovementFeatureAgent::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	CharacterPrivate = GetCharacter();
	check(CharacterPrivate.IsValid());

	if (CharacterPrivate.IsValid())
	{
		CharacterPrivate->MovementModeChangedDelegate.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementModeChanged);
	}

	if (UVSGameplayTagFeatureBase* GameplayTagFeature = GetPrimaryGameplayTagFeature_Native())
	{
		GameplayTagFeature->BindDelegateForObject(this);
	}
}

void UVSCharacterMovementFeatureAgent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	/** Sync current movement-mode state when character movement already began play. */
	if (CharacterPrivate.IsValid())
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
		{
			if (CharacterMovementComponent->HasBegunPlay())
			{
				OnCharacterMovementModeChanged(CharacterPrivate.Get(), MOVE_None, 0);
			}
		}
	}
}

void UVSCharacterMovementFeatureAgent::Uninitialize_Implementation()
{
	if (UVSGameplayTagFeatureBase* GameplayTagFeature = GetPrimaryGameplayTagFeature_Native())
	{
		GameplayTagFeature->UnbindDelegateForObject(this);
	}
	
	if (CharacterPrivate.IsValid())
	{
		CharacterPrivate->MovementModeChangedDelegate.RemoveDynamic(this, &UVSCharacterMovementFeatureAgent::OnCharacterMovementModeChanged);
	}

	CharacterPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

bool UVSCharacterMovementFeatureAgent::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && CharacterPrivate.IsValid();
}

void UVSCharacterMovementFeatureAgent::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	const FVector VelocityDelta = CharacterPrivate->GetVelocity() - GetCharacterMovement()->GetLastUpdateVelocity();
	const FRotator AngularDelta = (CharacterPrivate->GetActorRotation() - GetCharacterMovement()->GetLastUpdateRotation()).GetNormalized();
	
	RealMovementAcceleration = VelocityDelta / DeltaTime;
	RealMovementAngularVelocity = FVector(AngularDelta.Roll, AngularDelta.Pitch, AngularDelta.Yaw) / DeltaTime;
}

void UVSCharacterMovementFeatureAgent::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	Super::OnGameplayTagFeatureTagsUpdated_Implementation(GameplayTagFeature);

	for (UVSObjectFeature* Feature : GetSubFeatures())
	{
		if (Feature && Feature->IsA<UVSCharacterMovementFeature>() && Feature->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
		{
			Execute_OnGameplayTagFeatureTagsUpdated(Feature, GameplayTagFeature);
		}
	}
}

void UVSCharacterMovementFeatureAgent::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	Super::OnGameplayTagFeatureTagEventsNotified_Implementation(GameplayTagFeature, TagEvents);

	for (UVSObjectFeature* Feature : GetSubFeatures())
	{
		if (Feature && Feature->IsA<UVSCharacterMovementFeature>() && Feature->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
		{
			Execute_OnGameplayTagFeatureTagEventsNotified(Feature, GameplayTagFeature, TagEvents);
		}
	}
}

void UVSCharacterMovementFeatureAgent::OnCharacterMovementModeChanged(ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	const FGameplayTag PrevMovementModeTag = UVSCharacterMovementSystemUtils::GetTaggedMovementMode(PrevMovementMode, PreviousCustomMode);
	const FGameplayTag NewMovementModeTag = UVSCharacterMovementSystemUtils::GetTaggedMovementMode(InCharacter->GetCharacterMovement()->MovementMode, InCharacter->GetCharacterMovement()->CustomMovementMode);
}
