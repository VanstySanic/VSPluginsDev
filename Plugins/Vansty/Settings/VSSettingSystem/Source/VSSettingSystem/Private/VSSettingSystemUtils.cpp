// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemUtils.h"
#include "VSSettingSubsystem.h"

namespace
{
	TArray<UVSSettingItemBase*> ResolveSettingItemsFromIdentifiers(const FGameplayTagContainer& Identifiers)
	{
		TArray<UVSSettingItemBase*> ResolvedItems;

		for (const FGameplayTag& Identifier : Identifiers)
		{
			if (UVSSettingItemBase* SettingItem = UVSSettingSystemUtils::GetSettingItemByIdentifier(Identifier))
			{
				ResolvedItems.Add(SettingItem);
			}
		}

		return ResolvedItems;
	}
}

UVSSettingSystemUtils::UVSSettingSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSSettingSubsystem* UVSSettingSystemUtils::GetSettingSubsystemVS()
{
	return UVSSettingSubsystem::Get();
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetAllSettingItems()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		return Subsystem->GetSettingItems();
	}

	return TArray<UVSSettingItemBase*>();
}

TMap<FGameplayTag, UVSSettingItemBase*> UVSSettingSystemUtils::GetAllTaggedSettingItems()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		return Subsystem->GetTaggedSettingItems();
	}

	return TMap<FGameplayTag, UVSSettingItemBase*>();
}

FGameplayTagContainer UVSSettingSystemUtils::GetAllSettingItemIdentifiers()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		return Subsystem->GetSettingItemIdentifiers();
	}

	return FGameplayTagContainer::EmptyContainer;
}

UVSSettingItemBase* UVSSettingSystemUtils::GetSettingItemByIdentifier(const FGameplayTag& Identifier)
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		return Subsystem->GetSettingItemByIdentifier(Identifier);
	}

	return nullptr;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByIdentifierMatching(const FGameplayTag& Identifier)
{
	TArray<UVSSettingItemBase*> OutSettingItems;
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		for (UVSSettingItemBase* SettingItem : Subsystem->GetSettingItems())
		{
			if (SettingItem && SettingItem->GetItemIdentifier().MatchesTag(Identifier))
			{
				OutSettingItems.Add(SettingItem);
			}
		}
	}

	return OutSettingItems;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByTagQuery(const FGameplayTagQuery& Query)
{
	TArray<UVSSettingItemBase*> OutSettingItems;
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		for (UVSSettingItemBase* SettingItem : Subsystem->GetSettingItems())
		{
			if (SettingItem && Query.Matches(SettingItem->GetItemInfo().ItemTags))
			{
				OutSettingItems.Add(SettingItem);
			}
		}
	}

	return OutSettingItems;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::FilterRootSettingItems(const TArray<UVSSettingItemBase*>& Items)
{
	TArray<UVSSettingItemBase*> OutItems;
	TSet<UVSSettingItemBase*> AddedItems;

	for (UVSSettingItemBase* Item : Items)
	{
		if (!Item || AddedItems.Contains(Item)) continue;

		bool bHasAncestorInInput = false;
		for (UVSSettingItemBase* ParentItem = Item->GetTypedOuter<UVSSettingItemBase>(); ParentItem; ParentItem = ParentItem->GetTypedOuter<UVSSettingItemBase>())
		{
			if (Items.Contains(ParentItem))
			{
				bHasAncestorInInput = true;
				break;
			}
		}

		if (!bHasAncestorInInput)
		{
			OutItems.Add(Item);
			AddedItems.Add(Item);
		}
	}

	return OutItems;
}

FGameplayTagContainer UVSSettingSystemUtils::GetSettingItemIdentifiersByMatching(const FGameplayTag& Identifier)
{
	FGameplayTagContainer ItemIdentifiers;
	for (UVSSettingItemBase* SettingItem : GetSettingItemsByIdentifierMatching(Identifier))
	{
		if (SettingItem)
		{
			ItemIdentifiers.AddTag(SettingItem->GetItemIdentifier());
		}
	}

	return ItemIdentifiers;
}

FGameplayTagContainer UVSSettingSystemUtils::GetSettingItemIdentifiersByTagQuery(const FGameplayTagQuery& Query)
{
	FGameplayTagContainer ItemIdentifiers;
	for (UVSSettingItemBase* SettingItem : GetSettingItemsByTagQuery(Query))
	{
		if (SettingItem)
		{
			ItemIdentifiers.AddTag(SettingItem->GetItemIdentifier());
		}
	}
	
	return ItemIdentifiers;
}

FGameplayTagContainer UVSSettingSystemUtils::FilterRootSettingItemIdentifiers(const FGameplayTagContainer& Identifiers)
{
	FGameplayTagContainer OutIdentifiers;
	for (UVSSettingItemBase* Item : FilterRootSettingItems(ResolveSettingItemsFromIdentifiers(Identifiers)))
	{
		if (!Item) continue;

		const FGameplayTag ItemIdentifier = Item->GetItemIdentifier();
		if (ItemIdentifier.IsValid())
		{
			OutIdentifiers.AddTag(ItemIdentifier);
		}
	}

	return OutIdentifiers;
}

void UVSSettingSystemUtils::ExecuteSettingActionForItemIdentifiers(EVSSettingItemAction::Type Action, const FGameplayTagContainer& Identifiers)
{
	ExecuteSettingActionForItems(Action, ResolveSettingItemsFromIdentifiers(Identifiers));
}

void UVSSettingSystemUtils::ExecuteSettingActionForItems(EVSSettingItemAction::Type Action, const TArray<UVSSettingItemBase*>& Items)
{
	for (UVSSettingItemBase* SettingItem : FilterRootSettingItems(Items))
	{
		if (SettingItem)
		{
			SettingItem->ExecuteAction(Action);
		}
	}
}

void UVSSettingSystemUtils::ExecuteSettingActionsForItems(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions, const TArray<UVSSettingItemBase*>& Items)
{
	const TArray<UVSSettingItemBase*> FilteredItems = FilterRootSettingItems(Items);
	for (const EVSSettingItemAction::Type Action : Actions)
	{
		for (UVSSettingItemBase* SettingItem : FilteredItems)
		{
			if (SettingItem)
			{
				SettingItem->ExecuteAction(Action);
			}
		}
	}
}

void UVSSettingSystemUtils::ExecuteSettingActionsForItemIdentifiers(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions, const FGameplayTagContainer& Identifiers)
{
	ExecuteSettingActionsForItems(Actions, ResolveSettingItemsFromIdentifiers(Identifiers));
}

void UVSSettingSystemUtils::ExecuteSettingAction(EVSSettingItemAction::Type Action)
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->ExecuteSettingAction(Action);
	}
}

void UVSSettingSystemUtils::ExecuteSettingActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->ExecuteSettingActions(Actions);
	}
}

void UVSSettingSystemUtils::SetSettingsToBySource(EVSSettingItemValueSource::Type ValueSource)
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->SetSettingsToBySource(ValueSource);
	}
}

void UVSSettingSystemUtils::SetSettingsToBySourceForItemIdentifiers(EVSSettingItemValueSource::Type ValueSource, const FGameplayTagContainer& Identifiers)
{
	SetSettingsToBySourceForItems(ValueSource, ResolveSettingItemsFromIdentifiers(Identifiers));
}

void UVSSettingSystemUtils::SetSettingsToBySourceForItems(EVSSettingItemValueSource::Type ValueSource, const TArray<UVSSettingItemBase*>& Items)
{
	for (UVSSettingItemBase* SettingItem : FilterRootSettingItems(Items))
	{
		if (SettingItem)
		{
			SettingItem->SetToBySource(ValueSource);
		}
	}
}

void UVSSettingSystemUtils::LoadSettings()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->LoadSettings();
	}
}

void UVSSettingSystemUtils::ValidateSettings()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->ValidateSettings();
	}
}

void UVSSettingSystemUtils::ApplySettings()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->ApplySettings();
	}
}

void UVSSettingSystemUtils::ConfirmSettings()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->ConfirmSettings();
	}
}

void UVSSettingSystemUtils::SaveSettings()
{
	if (UVSSettingSubsystem* Subsystem = GetSettingSubsystemVS())
	{
		Subsystem->SaveSettings();
	}
}
