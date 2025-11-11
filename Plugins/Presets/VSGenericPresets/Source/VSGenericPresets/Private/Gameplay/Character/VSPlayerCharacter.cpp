// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Character/VSPlayerCharacter.h"
#include "Classes/Features/VSObjectFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "GameFramework/PlayerState.h"
#include "Libraries/VSActorLibrary.h"

AVSPlayerCharacter::AVSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	FeatureComponent->bRegisterOnBeginPlay = false;
}

void AVSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CheckExpectedTimeToSetup();
}

void AVSPlayerCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	CheckExpectedTimeToSetup();
}

void AVSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CheckExpectedTimeToSetup();
}

void AVSPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	CheckExpectedTimeToSetup();
}

UAbilitySystemComponent* AVSPlayerCharacter::GetAbilitySystemComponent() const
{
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Controller))
	{
		return Interface->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

void AVSPlayerCharacter::SetupAtExpectedTime_Implementation()
{
	if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
	{
		FeatureComponent->GetRootFeature()->RegisterFeature();	
	}
}

void AVSPlayerCharacter::CheckExpectedTimeToSetup()
{
	if (bHasBeenSetupAtTheBestTime && !HasActorBegunPlay() || !GetPlayerState()) return;
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(this) && !GetController()) return;
	bHasBeenSetupAtTheBestTime = true;
	SetupAtExpectedTime();
}

UVSGameplayTagController* AVSPlayerCharacter::GetGameplayTagController_Implementation() const
{
	if (GetPlayerState() && GetPlayerState()->GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(GetPlayerState());
	}

	return nullptr;
}
