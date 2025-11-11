// Copyright VanstySanic. All Rights Reserved.

#include "Gameplay/Pawn/VSPlayerPawn.h"

#include "Classes/Features/VSControlRotationFeature.h"
#include "Classes/Features/VSObjectFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Libraries/VSActorLibrary.h"

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

	CheckExpectedTimeToSetup();
}

void AVSPlayerPawn::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	CheckExpectedTimeToSetup();
}

void AVSPlayerPawn::OnRep_Controller()
{
	Super::OnRep_Controller();

	CheckExpectedTimeToSetup();
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

void AVSPlayerPawn::SetupAtExpectedTime_Implementation()
{
	if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
	{
		FeatureComponent->GetRootFeature()->RegisterFeature();	
	}
}

UVSGameplayTagController* AVSPlayerPawn::GetGameplayTagController_Implementation() const
{
	if (Controller && Controller.GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
	{
		return Execute_GetGameplayTagController(Controller);
	}

	return nullptr;
}

UVSControlRotationFeature* AVSPlayerPawn::GetControlRotationFeature_Implementation() const
{
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSControlRotationFeature>() : nullptr;
}

void AVSPlayerPawn::CheckExpectedTimeToSetup()
{
	if (bHasBeenSetupAtExpectedTime || !HasActorBegunPlay() || !GetPlayerState()) return;
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(this) && !GetController()) return;
	bHasBeenSetupAtExpectedTime = true;
	SetupAtExpectedTime();
}
