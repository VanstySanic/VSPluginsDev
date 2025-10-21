// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Character/VSAICharacter.h"
#include "AbilitySystemComponent.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"

AVSAICharacter::AVSAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* AVSAICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UVSGameplayTagController* AVSAICharacter::GetGameplayTagController_Implementation() const
{
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSGameplayTagController>() : nullptr;
}
