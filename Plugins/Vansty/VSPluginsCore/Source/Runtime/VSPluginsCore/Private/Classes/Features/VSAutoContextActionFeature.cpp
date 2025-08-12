// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSAutoContextActionFeature.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"

UVSAutoContextActionFeature::UVSAutoContextActionFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSAutoContextActionFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	GameplayTagControllerPrivate = UVSActorLibrary::GetGameplayTagControllerFromActor(GetOwnerActor());
	check(GameplayTagControllerPrivate.IsValid());

	PlayerControllerPrivate = Cast<APlayerController>(UVSGameplayLibrary::GetControllerFromSubObject(this));
	check(PlayerControllerPrivate.IsValid());

	EnhancedInputComponentPrivate = Cast<UEnhancedInputComponent>(PlayerControllerPrivate->InputComponent);
	check(EnhancedInputComponentPrivate.IsValid());
	
	LocalPlayerPrivate = PlayerControllerPrivate->GetLocalPlayer();
	check(LocalPlayerPrivate.IsValid());
	
	EnhancedInputLocalPlayerSubsystemPrivate = LocalPlayerPrivate->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EnhancedInputLocalPlayerSubsystemPrivate.IsValid());
	
	GameplayTagControllerPrivate->OnTagEventNotified.AddDynamic(this, &UVSAutoContextActionFeature::OnGameplayTagEventNotified);
}

void UVSAutoContextActionFeature::Uninitialize_Implementation()
{
	if (GameplayTagControllerPrivate.IsValid())
	{
		GameplayTagControllerPrivate->OnTagEventNotified.RemoveDynamic(this, &UVSAutoContextActionFeature::OnGameplayTagEventNotified);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSAutoContextActionFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	/** Add initial contexts. */
	for (const auto& QueriedContext : QueriedContexts)
	{
		if (!QueriedContext.Value.bAddByDefault || !QueriedContext.Key) continue;
		if (!EnhancedInputLocalPlayerSubsystemPrivate->HasMappingContext(QueriedContext.Key))
		{
			EnhancedInputLocalPlayerSubsystemPrivate->AddMappingContext(
				QueriedContext.Key,
				QueriedContext.Value.AddPriority,
				QueriedContext.Value.AddModifyOptions);
		}
	}
	OnGameplayTagEventNotified(EVSGameplayTagControllerTags::Event_TagsUpdated);
}

void UVSAutoContextActionFeature::OnGameplayTagEventNotified(const FGameplayTag& TagEvent)
{
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	const FGameplayTagContainer& GameplayTags = GameplayTagController->GetGameplayTags();
	
	for (const auto& QueriedContext : QueriedContexts)
	{
		if (!QueriedContext.Key) return;
		if (QueriedContext.Value.AutoRemoveQueries.Matches(GameplayTags, TagEvent))
		{
			if (EnhancedInputLocalPlayerSubsystemPrivate->HasMappingContext(QueriedContext.Key))
			{
				EnhancedInputLocalPlayerSubsystemPrivate->RemoveMappingContext(QueriedContext.Key);
				continue;
			}
		}
		if (QueriedContext.Value.AutoAddQueries.Matches(GameplayTags, TagEvent))
		{
			if (!EnhancedInputLocalPlayerSubsystemPrivate->HasMappingContext(QueriedContext.Key))
			{
				EnhancedInputLocalPlayerSubsystemPrivate->AddMappingContext(
					QueriedContext.Key,
					QueriedContext.Value.AddPriority,
					QueriedContext.Value.AddModifyOptions);
			}
		}
	}
}
