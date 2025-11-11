// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericPresets/Public/Gameplay/VSPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Libraries/VSActorLibrary.h"

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

void AVSPlayerState::SetupAtExpectedTime_Implementation()
{
	if (FeatureComponent && !FeatureComponent->bRegisterOnBeginPlay && !FeatureComponent->GetRootFeature()->IsRegistered())
	{
		FeatureComponent->GetRootFeature()->RegisterFeature();	
	}
}

UVSGameplayTagController* AVSPlayerState::GetGameplayTagController_Implementation() const
{
	UVSGameplayTagController* GameplayTagController = FeatureComponent->FindSubFeatureByClass<UVSGameplayTagController>();
	check(GameplayTagController);
	return FeatureComponent ? FeatureComponent->FindSubFeatureByClass<UVSGameplayTagController>() : nullptr;
}

void AVSPlayerState::CheckExpectedTimeToSetup()
{
	if (bHasBeenSetupAtExpectedTime || !HasActorBegunPlay()) return;
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(this) && !GetOwningController()) return;
	bHasBeenSetupAtExpectedTime = true;
	SetupAtExpectedTime();
}

void AVSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	CheckExpectedTimeToSetup();
}

void AVSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->SetOwnerActor(this);
	AbilitySystemComponent->SetAvatarActor(GetPawn());
}

void AVSPlayerState::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	CheckExpectedTimeToSetup();
}

void AVSPlayerState::OnRep_Owner()
{
	Super::OnRep_Owner();

	CheckExpectedTimeToSetup();
}

UAbilitySystemComponent* AVSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
