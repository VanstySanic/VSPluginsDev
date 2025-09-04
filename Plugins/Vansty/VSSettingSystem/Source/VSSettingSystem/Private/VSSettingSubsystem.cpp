// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystem/Public/VSSettingSubsystem.h"
#include "VSSettingSystemConfig.h"
#include "VSSettingSystemUtils.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingItemConfig.h"

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

UVSSettingSubsystem::UVSSettingSubsystem()
{
	
}

bool UVSSettingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), ChildClasses, false);
	
	return ChildClasses.Num() == 0;
}

void UVSSettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	check(GEngine);
	Scalability::LoadState(GGameUserSettingsIni);
	GEngine->GameUserSettingsClass = GEngine->GameUserSettingsClassName.TryLoadClass<UGameUserSettings>();

	for (const FSoftClassPath& SettingItemConfigClass : UVSSettingSystemConfig::Get()->SettingItemConfigClasses)
	{
		if (UClass* Class = SettingItemConfigClass.TryLoadClass<UVSSettingItemConfig>())
		{
			if (UVSSettingItemConfig* Config = NewObject<UVSSettingItemConfig>(this, Class))
			{
				for (UVSSettingItemBase* SettingItem : Config->GetSettingItems())
				{
					if (SettingItem)
					{
						SettingItems.Add(SettingItem);
					}
				}
			}
		}
	}

	for (TWeakObjectPtr<UVSSettingItemBase> SettingItem : SettingItems)
	{
		if (SettingItem.IsValid() && !SettingItem->HasBeenInitialized())
		{
			SettingItem->Initialize();
		}
	}

	UVSSettingSystemUtils::ExecuteActionsForSettingItems(GetSettingItems(), TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToCurrent,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Validate,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});

	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void UVSSettingSubsystem::Deinitialize()
{
	for (TWeakObjectPtr<UVSSettingItemBase> SettingItem : SettingItems)
	{
		if (SettingItem.IsValid() && SettingItem->HasBeenInitialized())
		{
			SettingItem->Uninitialize();
			SettingItem->MarkAsGarbage();
		}
	}
	SettingItems.Empty();
	
	Super::Deinitialize();
}
