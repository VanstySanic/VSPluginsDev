// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemAgent.h"
#include "VSSettingSubsystem.h"
#include "Types/Math/VSArray.h"

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
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemAgent, SubSettingItems))
	{
		if (!GetTypedOuter<UVSSettingItemAgent>() && EditorSubSettingItems != SubSettingItems)
		{
			if (UVSSettingSubsystem* Subsystem = UVSSettingSubsystem::Get())
			{
				const TArray<TObjectPtr<UVSSettingItem>> DifferentSettingItems = FVSArray::GetArrayDifference(EditorSubSettingItems, SubSettingItems);
				for (UVSSettingItem* DifferentSettingItem : DifferentSettingItems)
				{
					TArray<UVSSettingItem*> ItemsToAdd;
					TArray<UVSSettingItem*> ItemsToRemove;
					if (SubSettingItems.Contains(DifferentSettingItem))
					{
						ItemsToAdd.Add(DifferentSettingItem);
					}
					else
					{
						ItemsToRemove.Add(DifferentSettingItem);
					}
					Subsystem->RemoveEditorSettingItemDifferences(ItemsToRemove);
					Subsystem->AddEditorSettingItemDifferences(ItemsToAdd);
				}
			}
			EditorSubSettingItems = SubSettingItems;
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
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
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem && !SettingItem->EqualsToBySource(ValueSource))
		{
			return false;
		}
	}

	return true;
}

TArray<UVSSettingItem*> UVSSettingItemAgent::GetRecursiveSubSettingItems() const
{
	TArray<UVSSettingItem*> OutSettingItems = SubSettingItems;
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (UVSSettingItemAgent* Agent = Cast<UVSSettingItemAgent>(SettingItem))
		{
			OutSettingItems.Append(Agent->GetRecursiveSubSettingItems());
		}
	}
	return OutSettingItems;
}
