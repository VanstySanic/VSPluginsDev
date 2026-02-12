// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSEnhancedInputContextFeature.h"
#include "EnhancedInputSubsystems.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSActorLibrary.h"
#include "Classes/Libraries/VSObjectLibrary.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"

UVSEnhancedInputContextFeature::UVSEnhancedInputContextFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSEnhancedInputContextFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;
	
	PlayerControllerPrivate = Cast<APlayerController>(UVSActorLibrary::GetControllerFromActor(GetOwnerActor()));
	if (!PlayerControllerPrivate.IsValid() || !PlayerControllerPrivate->IsLocalController() || !PlayerControllerPrivate->GetLocalPlayer()) return;
	
	GameplayTagControllerPrivate = GetPrimaryGameplayTagFeature_Native();
	check(GameplayTagControllerPrivate.IsValid());
	GameplayTagControllerPrivate->BindDelegateForObject(this);
}

void UVSEnhancedInputContextFeature::Uninitialize_Implementation()
{
	if (GameplayTagControllerPrivate.IsValid())
	{
		GameplayTagControllerPrivate->UnbindDelegateForObject(this);
	}

	Super::Uninitialize_Implementation();
}

void UVSEnhancedInputContextFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (!LocalPlayerSubsystem) return;

	/** Add default contexts. */
	for (const auto& QueriedContext : QueriedContexts)
	{
		if (!QueriedContext.Key || !QueriedContext.Value.bAddByDefault) continue;
		if (!LocalPlayerSubsystem->HasMappingContext(QueriedContext.Key))
		{
			LocalPlayerSubsystem->AddMappingContext(
				QueriedContext.Key,
				QueriedContext.Value.AddPriority,
				QueriedContext.Value.AddModifyOptions);
		}
	}

	/** Initial refreshment. */
	RefreshContexts(FGameplayTagContainer::EmptyContainer);
}

UVSGameplayTagFeatureBase* UVSEnhancedInputContextFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagControllerPrivate.IsValid()) return GameplayTagControllerPrivate.Get();
	if (GetOwnerActor() && GetOwnerActor()->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
	{
		if (UVSGameplayTagFeatureBase* GameplayTagFeatureBase = Execute_GetPrimaryGameplayTagFeature(GetOwnerActor()))
		{
			return GameplayTagFeatureBase;	
		}
	}

	return UVSObjectLibrary::FindFeatureByClassFromObject<UVSGameplayTagFeatureBase>(GetOwnerActor());
}

void UVSEnhancedInputContextFeature::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	RefreshContexts(FGameplayTagContainer::EmptyContainer);
}

void UVSEnhancedInputContextFeature::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	RefreshContexts(TagEvents);
}

void UVSEnhancedInputContextFeature::RefreshContexts(const FGameplayTagContainer& TagEvents)
{
	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;
	if (!GameplayTagControllerPrivate.IsValid()) return;
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (!LocalPlayerSubsystem) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagControllerPrivate->GetGameplayTags();
	
	for (const auto& QueriedContext : QueriedContexts)
	{
		if (!QueriedContext.Key) continue;
		if (QueriedContext.Value.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (LocalPlayerSubsystem->HasMappingContext(QueriedContext.Key))
			{
				LocalPlayerSubsystem->RemoveMappingContext(QueriedContext.Key);
				continue;
			}
		}
		if (QueriedContext.Value.AutoAddTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (!LocalPlayerSubsystem->HasMappingContext(QueriedContext.Key))
			{
				LocalPlayerSubsystem->AddMappingContext(
					QueriedContext.Key,
					QueriedContext.Value.AddPriority,
					QueriedContext.Value.AddModifyOptions);
			}
		}
	}
}

UEnhancedInputLocalPlayerSubsystem* UVSEnhancedInputContextFeature::GetLocalPlayerSubsystem() const
{
	if (PlayerControllerPrivate.IsValid())
	{
		if (ULocalPlayer* LocalPlayer = PlayerControllerPrivate->GetLocalPlayer())
		{
			return LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		}
	}
	
	return nullptr;
}
