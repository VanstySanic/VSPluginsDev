// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItem.h"
#include "VSSettingSubsystem.h"

UVSSettingItem::UVSSettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorChangeActions.Add(EVSSettingItemAction::Apply);
	EditorChangeActions.Add(EVSSettingItemAction::Confirm);
#endif
}

#if WITH_EDITOR
void UVSSettingItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem, ItemTag))
	{
		if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
		{
			SettingSubsystem->RefreshEditorDirectSettingItemAgents();
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem::BeginDestroy()
{
	if (HasBeenInitialized())
	{
		Uninitialize();
		bHasBeenInitialized = false;
	}
	
	UObject::BeginDestroy();
}

void UVSSettingItem::Initialize_Implementation()
{

}

void UVSSettingItem::Uninitialize_Implementation()
{

}

void UVSSettingItem::Load_Implementation()
{
	
}

void UVSSettingItem::Validate_Implementation()
{
	
}

void UVSSettingItem::Apply_Implementation()
{
	
}

void UVSSettingItem::Confirm_Implementation()
{

}

void UVSSettingItem::Save_Implementation()
{
	
}

bool UVSSettingItem::IsUnconfirmed() const
{
	return !EqualsToBySource(EVSSettingItemValueSource::Confirmed);
}

void UVSSettingItem::SetToDefault()
{
	SetToBySource(EVSSettingItemValueSource::Default);
}

void UVSSettingItem::SetToGame()
{
	SetToBySource(EVSSettingItemValueSource::Game);
}

void UVSSettingItem::SetToConfirmed()
{
	SetToBySource(EVSSettingItemValueSource::Confirmed);
}

bool UVSSettingItem::IsValueValid_Implementation() const
{
	return true;
}

bool UVSSettingItem::IsDirty() const
{
	return !EqualsToBySource(EVSSettingItemValueSource::Confirmed);
}

void UVSSettingItem::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	
}

bool UVSSettingItem::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return false;
}

void UVSSettingItem::ExecuteAction(EVSSettingItemAction::Type Action)
{
	switch (Action)
	{
	case EVSSettingItemAction::Load:
		Load();
		break;
		
	case EVSSettingItemAction::SetToDefault:
		if (EqualsToBySource(EVSSettingItemValueSource::Default)) return;
		SetToBySource(EVSSettingItemValueSource::Default);
		break;
		
	case EVSSettingItemAction::SetToGame:
		if (EqualsToBySource(EVSSettingItemValueSource::Game)) return;
		SetToBySource(EVSSettingItemValueSource::Game);
		break;
		
	case EVSSettingItemAction::SetToConfirmed:
		if (EqualsToBySource(EVSSettingItemValueSource::Confirmed)) return;
		SetToBySource(EVSSettingItemValueSource::Confirmed);
		break;
	
	case EVSSettingItemAction::Apply:
		if (EqualsToBySource(EVSSettingItemValueSource::Game)) return;
		Apply();
		break;
		
	case EVSSettingItemAction::Confirm:
		if (!IsUnconfirmed()) return;
		Confirm();
		break;
		
	case EVSSettingItemAction::Save:
		Save();
		break;
		
	default: ;
	}
}

void UVSSettingItem::ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	for (const EVSSettingItemAction::Type Action : Actions)
	{
		ExecuteAction(Action);
	}
}

void UVSSettingItem::NotifyValueUpdated(bool bAllowCleanNotify)
{
	if (!HasBeenInitialized() || (!bAllowCleanNotify && !IsDirty()) || !ItemTag.IsValid()) return;
	
	OnValueUpdated();
	OnUpdated_Native.Broadcast(this);
	OnUpdated.Broadcast(this);

#if WITH_EDITOR
	if (!GIsPlayInEditorWorld)
	{
		ExecuteActions(EditorChangeActions);
	}
	else
#endif
	{
		ExecuteActions(InternalChangeActions);
	}
}

void UVSSettingItem::NotifyValueExternChanged(bool bAllowSameNotify)
{
	if (!HasBeenInitialized() || (!bAllowSameNotify && EqualsToBySource(EVSSettingItemValueSource::Game))) return;
	ExecuteActions(ExternalChangeActions);
}

bool UVSSettingItem::ShouldCreateSettingItem_Implementation() const
{
	return true;
}

void UVSSettingItem::OnValueUpdated_Implementation()
{
	
}

#if WITH_EDITOR
bool UVSSettingItem::EditorAllowChangingItemTag_Implementation() const
{
	return true;
}
#endif