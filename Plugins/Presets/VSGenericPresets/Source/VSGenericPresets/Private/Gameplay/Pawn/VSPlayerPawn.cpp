// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Pawn/VSPlayerPawn.h"
#include "Classes/Features/VSObjectFeatureComponent.h"

AVSPlayerPawn::AVSPlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
}

UAbilitySystemComponent* AVSPlayerPawn::GetAbilitySystemComponent() const
{
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Controller))
	{
		return Interface->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

UVSObjectFeatureComponent* AVSPlayerPawn::GetFeatureComponent() const
{
	return FeatureComponent;
}

UVSGameplayTagController* AVSPlayerPawn::GetGameplayTagController_Implementation() const
{
	if (Controller && Controller.GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(Controller);
	}

	return nullptr;
}