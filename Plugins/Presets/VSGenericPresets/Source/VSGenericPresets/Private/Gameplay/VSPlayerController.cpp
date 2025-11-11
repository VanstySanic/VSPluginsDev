// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericPresets/Public/Gameplay/VSPlayerController.h"
#include "VSPlayerCameraManager.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Gameplay/VSPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/VSActorLibrary.h"

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

	CheckExpectedTimeToSetup();
}

void AVSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CheckExpectedTimeToSetup();
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

void AVSPlayerController::SetupAtExpectedTime_Implementation()
{
	if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
	{
		FeatureComponent->GetRootFeature()->RegisterFeature();	
	}
}

UVSGameplayTagController* AVSPlayerController::GetGameplayTagController_Implementation() const
{
	if (PlayerState && PlayerState.GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(PlayerState);
	}
	
	return nullptr;
}

void AVSPlayerController::CheckExpectedTimeToSetup()
{
	if (bHasBeenSetupAtExpectedTime || !HasActorBegunPlay() || !PlayerState) return;
	bHasBeenSetupAtExpectedTime = true;
	SetupAtExpectedTime();
}
