// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericPresets/Public/Gameplay/VSPlayerController.h"
#include "VSPlayerCameraManager.h"
#include "Classes/Features/VSAutoContextActionFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Gameplay/VSPlayerState.h"

AVSPlayerController::AVSPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FeatureComponent = CreateDefaultSubobject<UVSObjectFeatureComponent>(TEXT("FeatureComponent"));
	FeatureComponent->AddDefaultSubFeatureByClass(this, UVSAutoContextActionFeature::StaticClass(), TEXT("AutoContextActionFeature"));

	PlayerCameraManagerClass = AVSPlayerCameraManager::StaticClass();
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
