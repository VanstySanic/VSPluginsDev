// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemUtils.h"
#include "VSSettingSubsystem.h"
#include "Items/VSSettingItemBase.h"

UVSSettingSystemUtils::UVSSettingSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItems()
{
	if (UVSSettingSubsystem* Subsystem = UVSSettingSubsystem::Get())
	{
		return Subsystem->GetSettingItems();
	}
	return TArray<UVSSettingItemBase*>();
}

UVSSettingItemBase* UVSSettingSystemUtils::GetSettingItemByIdentityTag(const FGameplayTag& IdentityTag)
{
	const TArray<UVSSettingItemBase*>& Items = GetSettingItems();

	for (UVSSettingItemBase* Item : Items)
	{
		if (Item->IdentityTag == IdentityTag)
		{
			return Item;
		}
	}

	return nullptr;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByCategoryTag(const FGameplayTag& CategoryTag)
{
	const TArray<UVSSettingItemBase*>& Items = GetSettingItems();
	TArray<UVSSettingItemBase*> OutItems = GetSettingItems();

	for (UVSSettingItemBase* Item : Items)
	{
		if (Item->CategoryTag == CategoryTag)
		{
			OutItems.Add(Item);
		}
	}

	return OutItems;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByTagQuery(const FGameplayTagQuery& TagQuery)
{
	const TArray<UVSSettingItemBase*>& Items = GetSettingItems();
	TArray<UVSSettingItemBase*> OutItems = GetSettingItems();

	for (UVSSettingItemBase* Item : Items)
	{
		if (TagQuery.Matches(Item->ItemTags))
		{
			OutItems.Add(Item);
		}
	}

	return OutItems;
}

void UVSSettingSystemUtils::ExecuteActionForSettingItems(const TArray<UVSSettingItemBase*>& SettingItems, const TEnumAsByte<EVSSettingItemAction::Type> Action)
{
	for (UVSSettingItemBase* SettingItem : SettingItems)
	{
		if (SettingItem)
		{
			SettingItem->ExecuteAction(Action);
		}
	}
}

void UVSSettingSystemUtils::ExecuteActionsForSettingItems(const TArray<UVSSettingItemBase*>& SettingItems, const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	for (TEnumAsByte<EVSSettingItemAction::Type> Action : Actions)
	{
		for (UVSSettingItemBase* SettingItem : SettingItems)
		{
			if (SettingItem)
			{
				SettingItem->ExecuteAction(Action);
			}
		}
	}
}
