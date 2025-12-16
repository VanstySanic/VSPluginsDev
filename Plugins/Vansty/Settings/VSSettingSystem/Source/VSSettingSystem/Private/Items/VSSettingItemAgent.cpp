// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemAgent.h"
#include "VSSettingSubsystem.h"
#include "Types/Math/VSArray.h"

UVSSettingItemAgent::UVSSettingItemAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItemAgent::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	EditorSubSettingItems = SubSettingItems;
#endif
}

#if WITH_EDITOR
void UVSSettingItemAgent::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	Super::PostCDOCompiled(Context);

	if (!Context.bIsRegeneratingOnLoad)
	{
		for (UVSSettingItem* RecursiveSubSettingItem : GetRecursiveSubSettingItems())
		{
			if (RecursiveSubSettingItem)
			{
				RecursiveSubSettingItem->PostLoad();
			}
		}
		
		if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
		{
			SettingSubsystem->RefreshEditorDirectSettingItemAgents();
		}
	}
}

void UVSSettingItemAgent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemAgent, SubSettingItems))
	{
		if (!GetTypedOuter<UVSSettingItemAgent>() && EditorSubSettingItems != SubSettingItems)
		{
			const TArray<UVSSettingItem*> DifferentSettingItems = FVSArray::GetArrayDifference(SubSettingItems, EditorSubSettingItems);
			for (UVSSettingItem* DifferentSettingItem : DifferentSettingItems)
			{
				if (DifferentSettingItem && SubSettingItems.Contains(DifferentSettingItem))
				{
					DifferentSettingItem->PostLoad();
				}
			}
			
			if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
			{
				SettingSubsystem->RefreshEditorDirectSettingItemAgents();
			}
			
			EditorSubSettingItems = SubSettingItems;
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemAgent::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem && !SettingItem->HasBeenInitialized())
		{
			SettingItem->Initialize();
			SettingItem->bHasBeenInitialized = true;
		}
	}
}

void UVSSettingItemAgent::Uninitialize_Implementation()
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem && SettingItem->HasBeenInitialized())
		{
			SettingItem->Uninitialize();
			//SettingItem->bHasBeenInitialized = false;
		}
	}

	Super::Uninitialize_Implementation();
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

bool UVSSettingItemAgent::IsValueValid_Implementation() const
{
	for (UVSSettingItem* SettingItem : SubSettingItems)
	{
		if (SettingItem && !SettingItem->IsValueValid())
		{
			return false;
		}
	}

	return true;
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
