// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystem/Public/VSSettingSubsystem.h"
#include "VSSettingSystemConfig.h"
#include "VSSettingSystemUtils.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingItemSet.h"

UVSSettingSubsystem* UVSSettingSubsystem::Get()
{
	return GEngine ? GEngine->GetEngineSubsystem<UVSSettingSubsystem>() : nullptr;
}

TArray<UVSSettingItemBase*> UVSSettingSubsystem::GetSettingItems() const
{
	TArray<UVSSettingItemBase*> OutSettingItems;
	for (TWeakObjectPtr<UVSSettingItemBase> SettingItemBase : SettingItems)
	{
		if (SettingItemBase.IsValid())
		{
			OutSettingItems.Add(SettingItemBase.Get());
		}
	}
	return OutSettingItems;
}

void UVSSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (const FSoftClassPath& SettingItemSetClass : UVSSettingSystemConfig::Get()->SettingItemSetClasses)
	{
		if (UClass* Class = SettingItemSetClass.ResolveClass())
		{
			if (UVSSettingItemSet* Set = NewObject<UVSSettingItemSet>(this, Class))
			{
				SettingItemSets.Add(Set);
			}
		}
	}

	for (TObjectPtr<UVSSettingItemSet> SettingItemSet : SettingItemSets)
	{
		for (UVSSettingItemBase* SettingItem : SettingItemSet->GetSettingItems())
		{
			if (SettingItem)
			{
				SettingItems.Add(SettingItem);
			}
		}
	}

	for (TWeakObjectPtr<UVSSettingItemBase> SettingItem : SettingItems)
	{
		SettingItem->Initialize();
	}

	UVSSettingSystemUtils::ExecuteActionsForSettingItems(GetSettingItems(), TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToCurrent,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Validate,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});
}

void UVSSettingSubsystem::Deinitialize()
{
	for (TWeakObjectPtr<UVSSettingItemBase> SettingItem : SettingItems)
	{
		if (SettingItem.IsValid())
		{
			SettingItem->Uninitialize();
		}
	}

	SettingItems.Empty();
	for (TObjectPtr<UVSSettingItemSet> SettingItemSet : SettingItemSets)
	{
		if (SettingItemSet)
		{
			SettingItemSet->MarkAsGarbage();
		}
	}
	SettingItemSets.Empty();
	
	Super::Deinitialize();
}
