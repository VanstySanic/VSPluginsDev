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

#if WITH_EDITOR
	bEditorHasBeenInitialized = true;
#endif
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
	for (const auto SettingItemAgentClass : UVSSettingSystemConfig::Get()->SettingItemAgentClasses)
	{
		if (UClass* AgentClass = SettingItemAgentClass.LoadSynchronous())
		{
			if (UVSSettingItemAgent* SettingItemAgent = AgentClass->GetDefaultObject<UVSSettingItemAgent>())
			{
				DirectSettingItemAgents.Add(SettingItemAgent);
			}
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		for (UVSSettingItem* SettingItem : SettingItemAgent->GetDirectSubSettingItems())
		{
			if (SettingItem)
			{
				SettingItems.Add(SettingItem);
				TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);
#if WITH_EDITOR
				EditorSettingItemTags.Add(SettingItem, SettingItem->GetItemTag());
#endif
			}
		}
	}

	for (UVSSettingItemAgent* SettingItemAgent : DirectSettingItemAgents)
	{
		SettingItemAgent->Initialize();
		SettingItemAgent->bHasBeenInitialized = true;
	}

	ExecuteSettingActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Validate,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});
}

#if WITH_EDITOR
void UVSSettingSubsystem::AddEditorSettingItem(UVSSettingItem* SettingItem)
{
	if (!bEditorHasBeenInitialized || !SettingItem || SettingItems.Contains(SettingItem)) return;
	
	SettingItems.Add(SettingItem);
	TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);
#if WITH_EDITOR
	EditorSettingItemTags.Add(SettingItem, SettingItem->GetItemTag());
#endif
	
	if (!SettingItem->HasBeenInitialized())
	{
		SettingItem->Initialize();
		SettingItem->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Validate,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});
	}
}

void UVSSettingSubsystem::RemoveEditorSettingItem(UVSSettingItem* SettingItem)
{
	if (!SettingItem || !SettingItems.Contains(SettingItem)) return;
	
	if (SettingItem->HasBeenInitialized())
	{
		SettingItem->Uninitialize();
	}
	
#if WITH_EDITOR
	EditorSettingItemTags.Remove(SettingItem);
#endif
	TaggedSettingItems.Remove(EditorSettingItemTags.FindRef(SettingItem));
	SettingItems.Remove(SettingItem);
	
	if (UVSSettingItemAgent* SettingItemAgent = Cast<UVSSettingItemAgent>(SettingItem))
	{
		if (DirectSettingItemAgents.Contains(SettingItemAgent))
		{
			DirectSettingItemAgents.Remove(SettingItemAgent);
		}
	}
}

void UVSSettingSubsystem::ReregisterEditorSettingItem(UVSSettingItem* SettingItem)
{
	if (!SettingItem || !SettingItems.Contains(SettingItem)) return;

	if (SettingItem->HasBeenInitialized())
	{
		SettingItem->Uninitialize();
		SettingItem->bHasBeenInitialized = false;
	}

	const FGameplayTag& PrevItemTag = EditorSettingItemTags.FindRef(SettingItem);
	TaggedSettingItems.Remove(PrevItemTag);
	TaggedSettingItems.Add(SettingItem->GetItemTag(), SettingItem);

	SettingItem->Initialize();

	SettingItem->ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::Load,
		EVSSettingItemAction::Validate,
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
	});
}
#endif
