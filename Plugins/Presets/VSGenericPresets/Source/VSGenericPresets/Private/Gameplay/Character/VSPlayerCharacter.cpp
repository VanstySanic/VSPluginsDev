// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Character/VSPlayerCharacter.h"

AVSPlayerCharacter::AVSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
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
	if (Controller && Controller.GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(Controller);
	}

	return nullptr;
}