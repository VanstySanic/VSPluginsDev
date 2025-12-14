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

	InitSettingItems();
}

void UVSSettingSubsystem::BeginDestroy()
{
	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		if (SettingItemAgent)
		{
			SettingItemAgent->Uninitialize();
		}
	}
	
	DirectSettingItemAgents.Empty();
	SettingItems.Empty();
	TaggedSettingItems.Empty();
#if WITH_EDITOR
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

void UVSSettingSubsystem::InitSettingItems()
{
	AddDirectSettingItemAgentClasses(UVSSettingSystemConfig::Get()->SettingItemAgentClasses);
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
				if (DirectSettingItemAgents.Contains(SettingItemAgent) || !SettingItemAgent->GetItemTag().IsValid()) continue;
				DirectSettingItemAgents.Add(SettingItemAgent);
				AgentsToAdd.Add(SettingItemAgent);
			}
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		if (!SettingItemAgent || !SettingItemAgent->GetItemTag().IsValid()) continue;
			
		SettingItems.Add(SettingItemAgent);
		TaggedSettingItems.Add(SettingItemAgent->GetItemTag(), SettingItemAgent);
#if WITH_EDITOR
		EditorSettingItemTags.Add(SettingItemAgent, SettingItemAgent->GetItemTag());
#endif
		
		for (UVSSettingItem* SettingItem : SettingItemAgent->GetRecursiveSubSettingItems())
		{
			if (!SettingItem || !SettingItem->GetItemTag().IsValid()) continue;
			SettingItems.Add(SettingItem);
			TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);
#if WITH_EDITOR
			EditorSettingItemTags.Add(SettingItem, SettingItem->GetItemTag());
#endif
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		SettingItemAgent->Initialize();
		SettingItemAgent->bHasBeenInitialized = true;
	}

	for (UVSSettingItemAgent* SettingItemAgent : AgentsToAdd)
	{
		SettingItemAgent->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToDefault,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
			EVSSettingItemAction::Save,
		});
	}
}

#if WITH_EDITOR
void UVSSettingSubsystem::RemoveEditorDirectSettingItemAgents(const TArray<TSoftClassPtr<UVSSettingItemAgent>>& SettingItemAgentClasses)
{
	TArray<UVSSettingItemAgent*> AgentsToRemove;
	
	for (const auto& SettingItemAgentClass : SettingItemAgentClasses)
	{
		if (UClass* AgentClass = SettingItemAgentClass.LoadSynchronous())
		{
			if (UVSSettingItemAgent* SettingItemAgent = AgentClass->GetDefaultObject<UVSSettingItemAgent>())
			{
				if (!DirectSettingItemAgents.Contains(SettingItemAgent)) continue;
				DirectSettingItemAgents.Remove(SettingItemAgent);
				AgentsToRemove.Add(SettingItemAgent);
			}
		}
	}
	
	for (UVSSettingItemAgent* SettingItemAgent : AgentsToRemove)
	{
		if (!SettingItemAgent) continue;

		const FGameplayTag AgentPrevTag = EditorSettingItemTags.FindRef(SettingItemAgent);
		TaggedSettingItems.Remove(AgentPrevTag);
		EditorSettingItemTags.Remove(SettingItemAgent);
		SettingItems.Remove(SettingItemAgent);
	
		for (UVSSettingItem* SettingItem : SettingItemAgent->GetRecursiveSubSettingItems())
		{
			if (SettingItem)
			{
				const FGameplayTag ItemPrevTag = EditorSettingItemTags.FindRef(SettingItem);
				TaggedSettingItems.Remove(ItemPrevTag);
				EditorSettingItemTags.Remove(SettingItem);
				SettingItems.Remove(SettingItem);
			}
		}

		if (SettingItemAgent->HasBeenInitialized())
		{
			SettingItemAgent->Uninitialize();
			SettingItemAgent->bHasBeenInitialized = false;
		}
	}
}

void UVSSettingSubsystem::AddEditorSettingItemDifferences(const TArray<UVSSettingItem*>& InSettingItems)
{
	TArray<UVSSettingItem*> TopLayerSettingItems;
	for (UVSSettingItem* SettingItem : InSettingItems)
	{
		if (!SettingItem || SettingItems.Contains(SettingItem) || !SettingItem->GetItemTag().IsValid()) continue;
	
		SettingItems.Add(SettingItem);
		TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);
		EditorSettingItemTags.Add(SettingItem, SettingItem->GetItemTag());
		TopLayerSettingItems.Add(SettingItem);

		if (UVSSettingItemAgent* Agent = Cast<UVSSettingItemAgent>(SettingItem))
		{
			for (UVSSettingItem* RecursiveSubSettingItem : Agent->GetRecursiveSubSettingItems())
			{
				if (!RecursiveSubSettingItem || !RecursiveSubSettingItem->GetItemTag().IsValid()) continue;

				SettingItems.Add(RecursiveSubSettingItem);
				TaggedSettingItems.Add(RecursiveSubSettingItem->GetItemTag(), RecursiveSubSettingItem);
				EditorSettingItemTags.Add(RecursiveSubSettingItem, RecursiveSubSettingItem->GetItemTag());
			}
		}
	}

	for (UVSSettingItem* TopLayerSettingItem : TopLayerSettingItems)
	{
		TopLayerSettingItem->Initialize();
		TopLayerSettingItem->bHasBeenInitialized = true;
	}

	for (UVSSettingItem* TopLayerSettingItem : TopLayerSettingItems)
	{
		TopLayerSettingItem->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToDefault,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
			EVSSettingItemAction::Save,
		});
	}
}

void UVSSettingSubsystem::RemoveEditorSettingItemDifferences(const TArray<UVSSettingItem*>& InSettingItems)
{
	for (UVSSettingItem* InSettingItem : InSettingItems)
	{
		if (!InSettingItem || !SettingItems.Contains(InSettingItem)) continue;
	
		if (InSettingItem->HasBeenInitialized())
		{
			InSettingItem->Uninitialize();
			InSettingItem->bHasBeenInitialized = false;
		}

		const FGameplayTag PrevTag = EditorSettingItemTags.FindRef(InSettingItem);
		TaggedSettingItems.Remove(PrevTag);
		EditorSettingItemTags.Remove(InSettingItem);
		SettingItems.Remove(InSettingItem);
	
		if (UVSSettingItemAgent* SettingItemAgent = Cast<UVSSettingItemAgent>(InSettingItem))
		{
			for (UVSSettingItem* RecursiveSubSettingItem : SettingItemAgent->GetRecursiveSubSettingItems())
			{
				if (!RecursiveSubSettingItem) continue;

				const FGameplayTag PrevSubTag = EditorSettingItemTags.FindRef(RecursiveSubSettingItem);
				TaggedSettingItems.Remove(PrevSubTag);
				EditorSettingItemTags.Remove(RecursiveSubSettingItem);
				SettingItems.Remove(RecursiveSubSettingItem);
			}
			
			if (DirectSettingItemAgents.Contains(SettingItemAgent))
			{
				DirectSettingItemAgents.Remove(SettingItemAgent);
			}
		}
	}
}

void UVSSettingSubsystem::ReregisterEditorSettingItemDifferences(const TArray<UVSSettingItem*>& InSettingItems)
{
	TArray<UVSSettingItem*> TopLayerItems;
	TArray<UVSSettingItem*> TopLayerItemsToAdd;
	TArray<UVSSettingItem*> TopLayerItemsToRemove;

	for (UVSSettingItem* InSettingItem : InSettingItems)
	{
		if (!InSettingItem) continue;

		if (!SettingItems.Contains(InSettingItem))
		{
			TopLayerItemsToAdd.Add(InSettingItem);
			continue;
		}

		if (!InSettingItem->GetItemTag().IsValid())
		{
			TopLayerItemsToRemove.Add(InSettingItem);
			continue;
		}

		if (InSettingItem->HasBeenInitialized())
		{
			InSettingItem->Uninitialize();
			InSettingItem->bHasBeenInitialized = false;
		}

		const FGameplayTag PrevItemTag = EditorSettingItemTags.FindRef(InSettingItem);
		TaggedSettingItems.Remove(PrevItemTag);
		TaggedSettingItems.Add(InSettingItem->GetItemTag(), InSettingItem);
		EditorSettingItemTags.Add(InSettingItem, InSettingItem->GetItemTag());
		TopLayerItems.Add(InSettingItem);

		if (UVSSettingItemAgent* SettingItemAgent = Cast<UVSSettingItemAgent>(InSettingItem))
		{
			for (UVSSettingItem* RecursiveSubSettingItem : SettingItemAgent->GetRecursiveSubSettingItems())
			{
				if (!RecursiveSubSettingItem) continue;

				const FGameplayTag PrevSubTag = EditorSettingItemTags.FindRef(RecursiveSubSettingItem);
				TaggedSettingItems.Remove(PrevSubTag);
				
				const FGameplayTag NewSubTag = RecursiveSubSettingItem->GetItemTag();
				if (NewSubTag.IsValid())
				{
					TaggedSettingItems.Add(NewSubTag, RecursiveSubSettingItem);
					EditorSettingItemTags.Add(RecursiveSubSettingItem, NewSubTag);
				}
				else
				{
					EditorSettingItemTags.Remove(RecursiveSubSettingItem);
				}
			}
		}
	}

	RemoveEditorSettingItemDifferences(TopLayerItemsToRemove);
	AddEditorSettingItemDifferences(TopLayerItemsToAdd);

	for (UVSSettingItem* SettingItem : TopLayerItems)
	{
		SettingItem->Initialize();
		SettingItem->bHasBeenInitialized = true;
	}

	for (UVSSettingItem* SettingItem : TopLayerItems)
	{
		SettingItem->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToDefault,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
			EVSSettingItemAction::Save,
		});
	}
}
#endif
