// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Character/VSPlayerCharacter.h"
#include "Classes/Features/VSObjectFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/PlayerState.h"

AVSPlayerCharacter::AVSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	FeatureComponent->bRegisterOnBeginPlay = false;
}

void AVSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetPlayerState())
	{
		if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
		{
			FeatureComponent->GetRootFeature()->RegisterFeature();	
		}
	}
}

void AVSPlayerCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	if (HasActorBegunPlay())
	{
		if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
		{
			FeatureComponent->GetRootFeature()->RegisterFeature();	
		}
	}
}

UAbilitySystemComponent* AVSPlayerCharacter::GetAbilitySystemComponent() const
{
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Controller))
	{
		return Interface->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

UVSGameplayTagController* AVSPlayerCharacter::GetGameplayTagController_Implementation() const
{
	if (GetPlayerState() && GetPlayerState()->GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(GetPlayerState());
	}

	return nullptr;
}