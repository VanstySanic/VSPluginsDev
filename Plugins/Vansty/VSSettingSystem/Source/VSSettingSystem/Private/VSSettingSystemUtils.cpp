// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemUtils.h"
#include "VSSettingSubsystem.h"
#include "Items/VSSettingItemBase.h"

UVSSettingSystemUtils::UVSSettingSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItems() const
{
	if (UVSSettingSubsystem* Subsystem = UVSSettingSubsystem::Get())
	{
		return Subsystem->GetSettingItems();
	}
	return TArray<UVSSettingItemBase*>();
}

UVSSettingItemBase* UVSSettingSystemUtils::GetSettingItemByIdentifyTag(const FGameplayTag& IdentifyTag) const
{
	const TArray<UVSSettingItemBase*>& Items = GetSettingItems();

	for (UVSSettingItemBase* Item : Items)
	{
		if (Item->IdentityTag == IdentifyTag)
		{
			return Item;
		}
	}

	return nullptr;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByCategoryTag(const FGameplayTag& CategoryTag) const
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

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByTagQuery(const FGameplayTagQuery& TagQuery) const
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
	for (UVSSettingItemBase* SettingItem : SettingItems)
	{
		if (SettingItem)
		{
			SettingItem->ExecuteActions(Actions);
		}
	}
}
