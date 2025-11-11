// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Character/VSCharacter.h"
#include "VSChrMovCapsuleComponent.h"
#include "Classes/Features/VSControlRotationFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Features/VSCharacterMovementAnimFeatureAgent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"

AVSCharacter::AVSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass(CapsuleComponentName, UVSChrMovCapsuleComponent::StaticClass()))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicateUsingRegisteredSubObjectList = true;

	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
	FeatureComponent->SetIsReplicated(true);
	FeatureComponent->GetRootFeature()->SetIsReplicated(true);
}

UAbilitySystemComponent* AVSCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

UVSObjectFeatureComponent* AVSCharacter::GetFeatureComponent() const
{
	return FeatureComponent;
}

UVSGameplayTagController* AVSCharacter::GetGameplayTagController_Implementation() const
{
	return nullptr;
}

UVSCharacterMovementFeatureAgent* AVSCharacter::GetMovementAgentFeature_Implementation() const
{
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSCharacterMovementFeatureAgent>() : nullptr;
}

UVSCharacterMovementAnimFeatureAgent* AVSCharacter::GetCharacterMovementAnimFeatureAgent_Implementation() const
{
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSCharacterMovementAnimFeatureAgent>() : nullptr;
}

UVSControlRotationFeature* AVSCharacter::GetControlRotationFeature_Implementation() const
{
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSControlRotationFeature>() : nullptr;
}