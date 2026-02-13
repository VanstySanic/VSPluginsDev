// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSEnhancedInputContextBinderFeature.h"
#include "EnhancedInputSubsystems.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSActorLibrary.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"

UVSEnhancedInputContextBinderFeature::UVSEnhancedInputContextBinderFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSEnhancedInputContextBinderFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	PlayerControllerPrivate = Cast<APlayerController>(UVSActorLibrary::GetControllerFromActor(GetOwnerActor()));
	if (PlayerControllerPrivate.IsValid() && PlayerControllerPrivate->IsLocalController())
	{
		GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
		if (GameplayTagFeaturePrivate.IsValid())
		{
			GameplayTagFeaturePrivate->BindDelegateForObject(this);
		}
	}
}

void UVSEnhancedInputContextBinderFeature::Uninitialize_Implementation()
{
	/** Remove all contexts. */
	if (PlayerControllerPrivate.IsValid())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem())
		{
			for (const TObjectPtr<UInputMappingContext>& Context : AddedContexts)
			{
				if (!Context) continue;
				const FVSEnhancedInputContextBinderSettings* Settings = ContextSettings.Find(Context);
				if (!Settings) continue;
				if (LocalPlayerSubsystem->HasMappingContext(Context))
				{
					LocalPlayerSubsystem->RemoveMappingContext(Context, Settings->RemoveModifyOptions);
				}
			}
		}

		if (GameplayTagFeaturePrivate.IsValid())
		{
			GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
		}
		
		AddedContexts.Reset();
		GameplayTagFeaturePrivate.Reset();
		PlayerControllerPrivate.Reset();
	}
	
	Super::Uninitialize_Implementation();
}

void UVSEnhancedInputContextBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (PlayerControllerPrivate.IsValid())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem())
		{
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
					AddedContexts.Add(Context.Key);
				}
			}
		}

		/** Initial refreshment. */
		RefreshContexts(FGameplayTagContainer::EmptyContainer);
	}
}

UVSGameplayTagFeatureBase* UVSEnhancedInputContextBinderFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagFeaturePrivate.IsValid()) return GameplayTagFeaturePrivate.Get();
	return IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation();
}

void UVSEnhancedInputContextBinderFeature::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	RefreshContexts(FGameplayTagContainer::EmptyContainer);
}

void UVSEnhancedInputContextBinderFeature::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	RefreshContexts(TagEvents);
}

void UVSEnhancedInputContextBinderFeature::RefreshContexts(const FGameplayTagContainer& TagEvents)
{
	if (!PlayerControllerPrivate.IsValid() || !GameplayTagFeaturePrivate.IsValid()) return;
	
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
				AddedContexts.Add(Context.Key);
			}
		}
		if (Context.Value.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (AddedContexts.Contains(Context.Key) && LocalPlayerSubsystem->HasMappingContext(Context.Key))
			{
				LocalPlayerSubsystem->RemoveMappingContext(Context.Key, Context.Value.RemoveModifyOptions);
				AddedContexts.Remove(Context.Key);
			}
			else if (AddedContexts.Contains(Context.Key) && !LocalPlayerSubsystem->HasMappingContext(Context.Key))
			{
				AddedContexts.Remove(Context.Key);
			}
		}
	}
}

UEnhancedInputLocalPlayerSubsystem* UVSEnhancedInputContextBinderFeature::GetLocalPlayerSubsystem() const
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
