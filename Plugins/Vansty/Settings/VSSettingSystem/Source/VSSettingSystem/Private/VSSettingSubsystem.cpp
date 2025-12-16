// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSubsystem.h"
#include "VSSettingSystemConfig.h"
#include "Items/VSSettingItemAgent.h"

UVSSettingSubsystem* UVSSettingSubsystem::Get()
{
	return GEngine ? GEngine->GetEngineSubsystem<UVSSettingSubsystem>() : nullptr;
}

void UVSSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AddDirectSettingItemAgentClasses(UVSSettingSystemConfig::Get()->SettingItemAgentClasses);
	
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToDefault,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});
	}
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
#if WITH_EDITORONLY_DATA
	EditorSettingItemTags.Empty();
#endif
	
	Super::BeginDestroy();
}

UVSSettingItem* UVSSettingSubsystem::GetSettingItemByTag(const FGameplayTag& ItemTag)
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
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->ExecuteActions(Actions);
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
				if (DirectSettingItemAgents.Contains(SettingItemAgent)) continue;
				DirectSettingItemAgents.Add(SettingItemAgent);
				AgentsToAdd.Add(SettingItemAgent);
			}
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		SettingItems.Add(SettingItemAgent);
		if (!SettingItemAgent->GetItemTag().IsValid())
		{
			TaggedSettingItems.Add(SettingItemAgent->GetItemTag(), SettingItemAgent);
#if WITH_EDITORONLY_DATA
			EditorSettingItemTags.Add(SettingItemAgent, SettingItemAgent->GetItemTag());
#endif
		}
		
		for (UVSSettingItem* SettingItem : SettingItemAgent->GetRecursiveSubSettingItems())
		{
			if (!SettingItem || !SettingItem->GetItemTag().IsValid()) continue;
			SettingItems.Add(SettingItem);
			TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);
#if WITH_EDITORONLY_DATA
			EditorSettingItemTags.Add(SettingItem, SettingItem->GetItemTag());
#endif
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		if (!SettingItemAgent->HasBeenInitialized())
		{
			SettingItemAgent->Initialize();
			SettingItemAgent->bHasBeenInitialized = true;
		}
	}

#if WITH_EDITOR
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});
	}
#endif
}

#if WITH_EDITOR
void UVSSettingSubsystem::ClearEditorDirectSettingItemAgents()
{
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
#if WITH_EDITORONLY_DATA
	EditorSettingItemTags.Empty();
#endif
}

void UVSSettingSubsystem::RefreshEditorDirectSettingItemAgents()
{
	ClearEditorDirectSettingItemAgents();
	AddDirectSettingItemAgentClasses(UVSSettingSystemConfig::Get()->SettingItemAgentClasses);
}
#endif
