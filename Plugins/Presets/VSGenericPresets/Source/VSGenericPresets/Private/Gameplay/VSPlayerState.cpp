// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericPresets/Public/Gameplay/VSPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"

AVSPlayerState::AVSPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicateUsingRegisteredSubObjectList = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
	FeatureComponent->SetIsReplicated(true);
	FeatureComponent->GetRootFeature()->SetIsReplicated(true);
}


UVSObjectFeatureComponent* AVSPlayerState::GetFeatureComponent() const
{
	return FeatureComponent;
}

UVSGameplayTagController* AVSPlayerState::GetGameplayTagController_Implementation() const
{
	UVSGameplayTagController* GameplayTagController = FeatureComponent->FindSubFeatureByClass<UVSGameplayTagController>();
	check(GameplayTagController);
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
