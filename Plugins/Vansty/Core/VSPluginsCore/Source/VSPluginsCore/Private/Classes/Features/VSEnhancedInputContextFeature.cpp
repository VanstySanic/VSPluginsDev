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
	
	GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
	check(GameplayTagFeaturePrivate.IsValid());
	GameplayTagFeaturePrivate->BindDelegateForObject(this);
}

void UVSEnhancedInputContextFeature::Uninitialize_Implementation()
{
	/** Remove all contexts. */
	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem())
		{
			for (const auto& Context : ContextSettings)
			{
				if (!Context.Key) continue;
				if (LocalPlayerSubsystem->HasMappingContext(Context.Key))
				{
					LocalPlayerSubsystem->RemoveMappingContext(Context.Key, Context.Value.AddModifyOptions);
				}
			}
		}
	}
	
	if (GameplayTagFeaturePrivate.IsValid())
	{
		GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
	}
	GameplayTagFeaturePrivate.Reset();
	PlayerControllerPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

void UVSEnhancedInputContextFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (!LocalPlayerSubsystem) return;

	/** Add default contexts. */
	for (const auto& Context : ContextSettings)
	{
		if (!Context.Key || !Context.Value.bAddByDefault) continue;
		if (!LocalPlayerSubsystem->HasMappingContext(Context.Key))
		{
			LocalPlayerSubsystem->AddMappingContext(
				Context.Key,
				Context.Value.AddPriority,
				Context.Value.AddModifyOptions);
		}
	}

	/** Initial refreshment. */
	RefreshContexts(FGameplayTagContainer::EmptyContainer);
}

UVSGameplayTagFeatureBase* UVSEnhancedInputContextFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagFeaturePrivate.IsValid()) return GameplayTagFeaturePrivate.Get();
	return IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation();
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
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (!LocalPlayerSubsystem) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (const auto& Context : ContextSettings)
	{
		if (!Context.Key) continue;
		if (Context.Value.AutoAddTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (!LocalPlayerSubsystem->HasMappingContext(Context.Key))
			{
				LocalPlayerSubsystem->AddMappingContext(
					Context.Key,
					Context.Value.AddPriority,
					Context.Value.AddModifyOptions);
			}
		}
		if (Context.Value.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (LocalPlayerSubsystem->HasMappingContext(Context.Key))
			{
				LocalPlayerSubsystem->RemoveMappingContext(Context.Key);
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
