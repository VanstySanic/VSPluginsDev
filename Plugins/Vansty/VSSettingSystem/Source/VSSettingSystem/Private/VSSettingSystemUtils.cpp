// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemUtils.h"
#include "VSSettingSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"
#include "Components/NativeWidgetHost.h"
#include "Components/TextBlock.h"
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

UVSSettingItemBase* UVSSettingSystemUtils::GetSettingItemBySpecifyTag(const FGameplayTag& SpecifyTag)
{
	const TArray<UVSSettingItemBase*>& Items = GetSettingItems();

	for (UVSSettingItemBase* Item : Items)
	{
		if (Item->GetItemInfo().SpecifyTag == SpecifyTag)
		{
			return Item;
		}
	}

	return nullptr;
}

TArray<UVSSettingItemBase*> UVSSettingSystemUtils::GetSettingItemsByCategoryTag(const FGameplayTag& CategoryTag, bool bExact)
{
	const TArray<UVSSettingItemBase*>& Items = GetSettingItems();
	TArray<UVSSettingItemBase*> OutItems = GetSettingItems();

	for (UVSSettingItemBase* Item : Items)
	{
		if (Item->GetItemInfo().CategoryTag == CategoryTag || (!bExact && Item->GetItemInfo().CategoryTag.MatchesTag(Item->GetItemInfo().CategoryTag)))
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
		if (TagQuery.Matches(Item->GetItemInfo().ItemTags))
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
