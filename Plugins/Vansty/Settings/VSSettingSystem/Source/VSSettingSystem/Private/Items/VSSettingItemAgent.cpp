// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemAgent.h"
#include "VSSettingSystemConfig.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

UVSSettingItemAgent::UVSSettingItemAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItemAgent::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITOR
	EditorSubSettingItems = SubSettingItems;
#endif
}

#if WITH_EDITOR
void UVSSettingItemAgent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemAgent, SubSettingItems))
	{
		if (!GetTypedOuter<UVSSettingItemAgent>() && EditorSubSettingItems != SubSettingItems)
		{
			if (!UVSSettingSystemConfig::Get()->bIgnoreEditorSettingItemArrayNotification)
			{
				FNotificationInfo Info(NSLOCTEXT("VSSettingSystem.SettingItemAgent", "SettingItemsChanging", "An engine restart might be executed because of the changing setting items."));
				Info.bFireAndForget = true;
				Info.bUseLargeFont = false;
				Info.FadeInDuration = 0.25f;
				Info.FadeOutDuration = 0.25f;
				Info.ExpireDuration = 5.f;
				FSlateNotificationManager::Get().AddNotification(Info);

				EditorSubSettingItems = SubSettingItems;
			}
		}
	}
}
#endif

void UVSSettingItemAgent::Initialize_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Initialize();
		}
	}
}

void UVSSettingItemAgent::Uninitialize_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Uninitialize();
		}
	}
}

void UVSSettingItemAgent::Load_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Load();
		}
	}
}

void UVSSettingItemAgent::Validate_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Validate();
		}
	}
}

void UVSSettingItemAgent::Apply_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Apply();
		}
	}
}

void UVSSettingItemAgent::Confirm_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Confirm();
		}
	}
}

void UVSSettingItemAgent::Save_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->Save();
		}
	}
}

void UVSSettingItemAgent::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			SettingItem->SetToBySource(ValueSource);
		}
	}
}

bool UVSSettingItemAgent::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	bool bNotEqual = false;
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem)
		{
			bNotEqual = bNotEqual ? true : !SettingItem->EqualsToBySource(ValueSource);
		}
	}

	return !bNotEqual;
}
