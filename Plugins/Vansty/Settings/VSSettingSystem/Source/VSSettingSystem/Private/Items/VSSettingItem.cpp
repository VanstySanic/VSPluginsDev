// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItem.h"
#include "VSSettingSubsystem.h"
#include "Classes/Libraries/VSObjectLibrary.h"
#include "Items/VSSettingItemAgent.h"

UVSSettingItem::UVSSettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

#if WITH_EDITOR
void UVSSettingItem::PreEditChange(class FEditPropertyChain& PropertyAboutToChange)
{
	if (HasBeenInitialized())
	{
		Uninitialize();
	}

	Super::PreEditChange(PropertyAboutToChange);
}

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
	SetToBySource(EVSSettingItemValueSource::System);
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
	return !EqualsToBySource(EVSSettingItemValueSource::Game);
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
		if (!IsDirty()) return;
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

void UVSSettingItem::NotifyValueUpdate(bool bAllowCleanNotify)
{
	if (!bAllowCleanNotify && !IsDirty()) return;

	OnValueUpdated();
	OnUpdated_Native.Broadcast(this);
	OnUpdated.Broadcast(this);
}

void UVSSettingItem::OnValueUpdated_Implementation()
{
	
}

UVSSettingItemAgent* UVSSettingItem::GetRootMostAgent() const
{
	if (UVSSettingItemAgent* Agent = GetTypedOuter<UVSSettingItemAgent>())
	{
		return Agent->GetRootMostAgent();
	}

	return IsA<UVSSettingItemAgent>() ? Cast<UVSSettingItemAgent>(const_cast<UVSSettingItem*>(this)) : nullptr;
}


#if WITH_EDITOR
bool UVSSettingItem::AllowChangingItemTag_Implementation() const
{
	return true;
}
#endif