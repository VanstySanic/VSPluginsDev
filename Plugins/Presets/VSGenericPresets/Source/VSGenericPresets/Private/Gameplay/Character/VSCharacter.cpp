// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Character/VSCharacter.h"
#include "VSChrMovCapsuleComponent.h"
#include "Classes/Features/VSObjectFeatureComponent.h"

AVSCharacter::AVSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass(CapsuleComponentName, UVSChrMovCapsuleComponent::StaticClass()))
{
	PrimaryActorTick.bCanEverTick = true;

	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
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
