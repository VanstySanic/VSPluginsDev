// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericPresets/Public/Gameplay/VSPlayerController.h"
#include "VSPlayerCameraManager.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Gameplay/VSPlayerState.h"
#include "Kismet/GameplayStatics.h"

AVSPlayerController::AVSPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicateUsingRegisteredSubObjectList = true;
	PlayerCameraManagerClass = AVSPlayerCameraManager::StaticClass();

	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
	FeatureComponent->GetRootFeature()->SetIsReplicated(true);
	FeatureComponent->bRegisterOnBeginPlay = false;
}

void AVSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerState)
	{
		if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
		{
			FeatureComponent->GetRootFeature()->RegisterFeature();	
		}
	}
}

void AVSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (HasActorBegunPlay())
	{
		if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
		{
			FeatureComponent->GetRootFeature()->RegisterFeature();
		}
	}
}

UAbilitySystemComponent* AVSPlayerController::GetAbilitySystemComponent() const
{
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(PlayerState))
	{
		return Interface->GetAbilitySystemComponent();
	}
	return nullptr;
}

UVSObjectFeatureComponent* AVSPlayerController::GetFeatureComponent() const
{
	return FeatureComponent;
}

UVSGameplayTagController* AVSPlayerController::GetGameplayTagController_Implementation() const
{
	if (PlayerState && PlayerState.GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(PlayerState);
	}
	
	return nullptr;
}
