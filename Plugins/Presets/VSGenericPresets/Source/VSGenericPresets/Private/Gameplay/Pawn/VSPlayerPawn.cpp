// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Pawn/VSPlayerPawn.h"

#include "Classes/Features/VSObjectFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"

AVSPlayerPawn::AVSPlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicateUsingRegisteredSubObjectList = true;
	
	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
	FeatureComponent->SetIsReplicated(true);
	FeatureComponent->bRegisterOnBeginPlay = false;
}

void AVSPlayerPawn::BeginPlay()
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

void AVSPlayerPawn::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
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