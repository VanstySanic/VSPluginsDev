// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericPresets/Public/Gameplay/VSPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"

AVSPlayerState::AVSPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
	FeatureComponent->AddDefaultSubFeatureByClass(this, UVSGameplayTagController::StaticClass());
}


UVSObjectFeatureComponent* AVSPlayerState::GetFeatureComponent() const
{
	return FeatureComponent;
}

UVSGameplayTagController* AVSPlayerState::GetGameplayTagController_Implementation() const
{
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSGameplayTagController>() : nullptr;
}

void AVSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->SetOwnerActor(this);
	AbilitySystemComponent->SetAvatarActor(GetPawn());
}

UAbilitySystemComponent* AVSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
