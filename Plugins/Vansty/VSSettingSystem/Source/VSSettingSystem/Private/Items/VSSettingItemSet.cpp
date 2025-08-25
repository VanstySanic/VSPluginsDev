// Copyright VanstySanic. All Rights Reserved.


#include "Items/VSSettingItemSet.h"

UVSSettingItemSet::UVSSettingItemSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<UVSSettingItemBase*> UVSSettingItemSet::GetSettingItems() const
{
	TArray<UVSSettingItemBase*> OutSettingItems;
	for (const FVSSettingItemGroup& SettingItemGroup : SettingItemGroups)
	{
		OutSettingItems.Append(SettingItemGroup.SettingItems);
	}
	return OutSettingItems;
}
