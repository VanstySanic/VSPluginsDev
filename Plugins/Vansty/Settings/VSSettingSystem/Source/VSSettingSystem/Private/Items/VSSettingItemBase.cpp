// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemBase.h"
#include "VSSettingSubsystem.h"

UVSSettingItemBase::UVSSettingItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorChangeActions.Add(EVSSettingItemAction::Apply);
	EditorChangeActions.Add(EVSSettingItemAction::Confirm);
#endif
}

#if WITH_EDITOR
void UVSSettingItemBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase, ItemIdentifier))
	{
		if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
		{
			SettingSubsystem->RefreshEditorDirectSettingItemAgents();
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemBase::BeginDestroy()
{
	if (HasBeenInitialized())
	{
		Uninitialize();
		bHasBeenInitialized = false;
	}
	
	UObject::BeginDestroy();
}

void UVSSettingItemBase::Initialize_Implementation()
{

}

void UVSSettingItemBase::Uninitialize_Implementation()
{

}

void UVSSettingItemBase::Load_Implementation()
{
	
}

void UVSSettingItemBase::Validate_Implementation()
{
	
}

void UVSSettingItemBase::Apply_Implementation()
{
	
}

void UVSSettingItemBase::Confirm_Implementation()
{

}

void UVSSettingItemBase::Save_Implementation()
{
	
}

bool UVSSettingItemBase::IsUnconfirmed() const
{
	return !EqualsToBySource(EVSSettingItemValueSource::Confirmed);
}

void UVSSettingItemBase::SetToDefault()
{
	SetToBySource(EVSSettingItemValueSource::Default);
}

void UVSSettingItemBase::SetToGame()
{
	SetToBySource(EVSSettingItemValueSource::Game);
}

void UVSSettingItemBase::SetToConfirmed()
{
	SetToBySource(EVSSettingItemValueSource::Confirmed);
}

bool UVSSettingItemBase::IsValueValid_Implementation() const
{
	return true;
}

bool UVSSettingItemBase::IsDirty() const
{
	return !EqualsToBySource(EVSSettingItemValueSource::Confirmed);
}

void UVSSettingItemBase::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	
}

bool UVSSettingItemBase::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return false;
}

void UVSSettingItemBase::ExecuteAction(EVSSettingItemAction::Type Action)
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

void UVSSettingItemBase::ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	for (const EVSSettingItemAction::Type Action : Actions)
	{
		ExecuteAction(Action);
	}
}

void UVSSettingItemBase::NotifyValueUpdated(bool bAllowCleanNotify)
{
	if (!HasBeenInitialized() || (!bAllowCleanNotify && !IsDirty()) || !ItemIdentifier.IsValid()) return;
	
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

void UVSSettingItemBase::NotifyValueExternChanged(bool bAllowSameNotify)
{
	if (!HasBeenInitialized() || (!bAllowSameNotify && EqualsToBySource(EVSSettingItemValueSource::Game))) return;
	ExecuteActions(ExternalChangeActions);
}

bool UVSSettingItemBase::ShouldCreateSettingItem_Implementation() const
{
	return true;
}

void UVSSettingItemBase::OnValueUpdated_Implementation()
{
	
}

#if WITH_EDITOR
bool UVSSettingItemBase::EditorAllowChangingItemIdentifier_Implementation() const
{
	return true;
}
#endif