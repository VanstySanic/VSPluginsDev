// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemAgent.h"
#include "VSSettingSubsystem.h"

UVSSettingItemAgent::UVSSettingItemAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UVSSettingItemAgent::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	Super::PostCDOCompiled(Context);
	
	if (!Context.bIsRegeneratingOnLoad)
	{
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
			EditorSubSettingItems = SubSettingItems;
			if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
			{
				SettingSubsystem->RefreshEditorDirectSettingItemAgents();
			}
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemAgent::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	EditorSubSettingItems = SubSettingItems;
#endif
}

void UVSSettingItemAgent::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
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
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem && SettingItem->HasBeenInitialized())
		{
			SettingItem->Uninitialize();
			SettingItem->bHasBeenInitialized = false;
		}
	}

	Super::Uninitialize_Implementation();
}

void UVSSettingItemAgent::Load_Implementation()
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem)
		{
			SettingItem->Load();
		}
	}
}

void UVSSettingItemAgent::Validate_Implementation()
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem)
		{
			SettingItem->Validate();
		}
	}
}

void UVSSettingItemAgent::Apply_Implementation()
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem)
		{
			SettingItem->Apply();
		}
	}
}

void UVSSettingItemAgent::Confirm_Implementation()
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem)
		{
			SettingItem->Confirm();
		}
	}
}

void UVSSettingItemAgent::Save_Implementation()
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem)
		{
			SettingItem->Save();
		}
	}
}

bool UVSSettingItemAgent::IsValueValid_Implementation() const
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
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
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem)
		{
			SettingItem->SetToBySource(ValueSource);
		}
	}
}

bool UVSSettingItemAgent::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (SettingItem && !SettingItem->EqualsToBySource(ValueSource))
		{
			return false;
		}
	}

	return true;
}

TArray<UVSSettingItemBase*> UVSSettingItemAgent::GetDirectSubSettingItems() const
{
	TArray<UVSSettingItemBase*> OutItems;
	for (UVSSettingItemBase* SettingItem : SubSettingItems)
	{
		if (SettingItem && SettingItem->GetItemTag().IsValid() && SettingItem->ShouldCreateSettingItem())
		{
			OutItems.Add(SettingItem);
		}
	}

	return OutItems;
}

TArray<UVSSettingItemBase*> UVSSettingItemAgent::GetRecursiveSubSettingItems() const
{
	TArray<UVSSettingItemBase*> OutSettingItems = SubSettingItems;
	for (UVSSettingItemBase* SettingItem : GetDirectSubSettingItems())
	{
		if (!SettingItem || !SettingItem->GetItemTag().IsValid() || !SettingItem->ShouldCreateSettingItem()) continue;
		if (UVSSettingItemAgent* Agent = Cast<UVSSettingItemAgent>(SettingItem))
		{
			OutSettingItems.Append(Agent->GetRecursiveSubSettingItems());
		}
	}
	return OutSettingItems;
}
