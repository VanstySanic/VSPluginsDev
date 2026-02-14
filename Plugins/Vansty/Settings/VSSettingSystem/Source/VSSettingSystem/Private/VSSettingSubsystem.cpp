// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSubsystem.h"
#include "VSSettingSystemConfig.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingItemAgent.h"

UVSSettingSubsystem* UVSSettingSubsystem::Get()
{
	return GEngine ? GEngine->GetEngineSubsystem<UVSSettingSubsystem>() : nullptr;
}

void UVSSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	/** This is the best time to initialize settings. */
	FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddWeakLambda(this, [this] ()
	{
		if (GEngine && !GEngine->GameUserSettingsClass)
		{
			GEngine->GameUserSettingsClass = GEngine->GameUserSettingsClassName.TryLoadClass<UGameUserSettings>();
		}
		
		AddDirectSettingItemAgentClasses(UVSSettingSystemConfig::Get()->SettingItemAgentClasses);
		
		FCoreDelegates::OnAllModuleLoadingPhasesComplete.RemoveAll(this);
	});
}

void UVSSettingSubsystem::BeginDestroy()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		if (SettingItemAgent && SettingItemAgent->HasBeenInitialized())
		{
			SettingItemAgent->Uninitialize();
			SettingItemAgent->bHasBeenInitialized = false;
		}
	}
	
	DirectSettingItemAgents.Empty();
	SettingItems.Empty();
	TaggedSettingItems.Empty();
	
	Super::BeginDestroy();
}

bool UVSSettingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

UVSSettingItemBase* UVSSettingSubsystem::GetSettingItemByTag(const FGameplayTag& ItemTag)
{
	return TaggedSettingItems.FindRef(ItemTag).Get();
}

void UVSSettingSubsystem::LoadSettings()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->Load();
	}
}

void UVSSettingSubsystem::ValidateSettings()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->Validate();
	}
}

void UVSSettingSubsystem::ApplySettings()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->Apply();
	}
}

void UVSSettingSubsystem::ConfirmSettings()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->Confirm();
	}
}

void UVSSettingSubsystem::SaveSettings()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->Save();
	}
}


void UVSSettingSubsystem::ExecuteSettingAction(EVSSettingItemAction::Type Action)
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->ExecuteAction(Action);
	}
}

void UVSSettingSubsystem::ExecuteSettingActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	for (TEnumAsByte<EVSSettingItemAction::Type> Action : Actions)
	{
		for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
		{
			SettingItemAgent->ExecuteAction(Action);
		}
	}
}

void UVSSettingSubsystem::AddDirectSettingItemAgentClasses(const TArray<TSoftClassPtr<UVSSettingItemAgent>>& SettingItemAgentClasses)
{
	TArray<UVSSettingItemAgent*> AgentsToAdd;
	
	for (const auto& SettingItemAgentClass : SettingItemAgentClasses)
	{
		if (UClass* AgentClass = SettingItemAgentClass.LoadSynchronous())
		{
			if (UVSSettingItemAgent* SettingItemAgent = AgentClass->GetDefaultObject<UVSSettingItemAgent>())
			{
				if (!SettingItemAgent || !SettingItemAgent->ShouldCreateSettingItem()) continue;
				if (DirectSettingItemAgents.Contains(SettingItemAgent)) continue;
				DirectSettingItemAgents.Add(SettingItemAgent);
				AgentsToAdd.Add(SettingItemAgent);
			}
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		SettingItems.Add(SettingItemAgent);
		if (SettingItemAgent->GetItemTag().IsValid())
		{
			TaggedSettingItems.Add(SettingItemAgent->GetItemTag(), SettingItemAgent);
		}
		
		for (UVSSettingItemBase* SettingItem : SettingItemAgent->GetRecursiveSubSettingItems())
		{
			if (!SettingItem || !SettingItem->GetItemTag().IsValid() || !SettingItem->ShouldCreateSettingItem()) continue;

			SettingItems.Add(SettingItem);
			TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);
		}
	}

	/** Cache actions. */
	TMap<UVSSettingItemBase*, TArray<TEnumAsByte<EVSSettingItemAction::Type>>> InternalChangeActions;
	for (UVSSettingItemBase* SettingItem : SettingItems)
	{
		InternalChangeActions.Add(SettingItem, SettingItem->InternalChangeActions);
		SettingItem->InternalChangeActions.Empty();
	}
	
	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		SettingItemAgent->Initialize();
		SettingItemAgent->bHasBeenInitialized = true;
	}

	/** Execute actions after initialization and restore prev actions. */
	TArray<TEnumAsByte<EVSSettingItemAction::Type>> InitActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::SetToDefault,
		EVSSettingItemAction::SetToGame,
		EVSSettingItemAction::Load,
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
	};
	for (EVSSettingItemAction::Type InitAction : InitActions)
	{
		for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
		{
			SettingItemAgent->ExecuteAction(InitAction);
		}
	}
	
	for (UVSSettingItemBase* SettingItem : SettingItems)
	{
		SettingItem->InternalChangeActions = InternalChangeActions.FindRef(SettingItem);
	}

	for (UVSSettingItemBase* SettingItem : SettingItems)
	{
		SettingItem->OnUpdated_Native.AddUObject(this, &UVSSettingSubsystem::OnSettingItemUpdated);
	}
}

#if WITH_EDITOR
void UVSSettingSubsystem::ClearEditorDirectSettingItemAgents()
{
	for (UVSSettingItemBase* SettingItem : SettingItems)
	{
		if (SettingItem)
		{
			SettingItem->OnUpdated_Native.RemoveAll(this);
		}
	}
	
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		if (SettingItemAgent && SettingItemAgent->HasBeenInitialized())
		{
			SettingItemAgent->Uninitialize();
			SettingItemAgent->bHasBeenInitialized = false;
		}
	}

	SettingItems.Empty();
	TaggedSettingItems.Empty();
	DirectSettingItemAgents.Empty();
}

void UVSSettingSubsystem::RefreshEditorDirectSettingItemAgents()
{
	ClearEditorDirectSettingItemAgents();
	AddDirectSettingItemAgentClasses(UVSSettingSystemConfig::Get()->SettingItemAgentClasses);
}
#endif

void UVSSettingSubsystem::OnSettingItemUpdated(UVSSettingItemBase* SettingItem)
{
	OnItemUpdated_Native.Broadcast(SettingItem);
	OnItemUpdated.Broadcast(SettingItem);
}
