// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystem/Public/Items/VSSettingItemBase.h"
#include "VSSettingSubsystem.h"

UVSSettingItemBase::UVSSettingItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItemBase::Initialize_Implementation()
{

}

void UVSSettingItemBase::Uninitialize_Implementation()
{
	
}

FGameplayTagContainer UVSSettingItemBase::GetSettingItemTags()
{
	FGameplayTagContainer ComposedTags = ItemTags;
	ComposedTags.AddTag(IdentityTag);
	ComposedTags.AddTag(CategoryTag);
	return ComposedTags;
}

void UVSSettingItemBase::ExecuteAction(TEnumAsByte<EVSSettingItemAction::Type> Action)
{
	switch (Action) {
	case EVSSettingItemAction::None:
		return;
		
	case EVSSettingItemAction::Load:
		Load();
		break;
		
	case EVSSettingItemAction::SetToDefault:
		if (EqualsToByValueType(EVSSettingItemValueType::Default)) return;
		SetToByValueType(EVSSettingItemValueType::Default);
		break;
		
	case EVSSettingItemAction::SetToCurrent:
		if (EqualsToByValueType(EVSSettingItemValueType::Current)) return;
		SetToByValueType(EVSSettingItemValueType::Current);
		break;
		
	case EVSSettingItemAction::SetToLastConfirmed:
		if (EqualsToByValueType(EVSSettingItemValueType::LastConfirmed)) return;
		SetToByValueType(EVSSettingItemValueType::LastConfirmed);
		break;

	case EVSSettingItemAction::Validate:
		Validate();
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

	OnActionExecuted.Broadcast(Action);
	if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
	{
		SettingSubsystem->OnSettingItemActionExecuted.Broadcast(this, Action);
	}

	switch (Action) {
	case EVSSettingItemAction::Load:
	case EVSSettingItemAction::SetToDefault:
	case EVSSettingItemAction::SetToCurrent:
	case EVSSettingItemAction::SetToLastConfirmed:
	case EVSSettingItemAction::Validate:
	case EVSSettingItemAction::Confirm:
		NotifyUpdate();
		break;

	default: ;
	}
}

void UVSSettingItemBase::ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	for (const TEnumAsByte<EVSSettingItemAction::Type>& Action : Actions)
	{
		ExecuteAction(Action);
	}
}

void UVSSettingItemBase::NotifyUpdate()
{
	OnUpdated.Broadcast();
	if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
	{
		SettingSubsystem->OnSettingItemUpdated.Broadcast(this);
	}
}

void UVSSettingItemBase::SetToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType)
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

bool UVSSettingItemBase::EqualsToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) const
{
	return false;
}

bool UVSSettingItemBase::IsDirty() const
{
	return EqualsToByValueType(EVSSettingItemValueType::Settings) != EqualsToByValueType(EVSSettingItemValueType::Current);
}

bool UVSSettingItemBase::IsDefault() const
{
	return EqualsToByValueType(EVSSettingItemValueType::Settings) == EqualsToByValueType(EVSSettingItemValueType::Default);
}

bool UVSSettingItemBase::IsUnconfirmed() const
{
	return EqualsToByValueType(EVSSettingItemValueType::Settings) != EqualsToByValueType(EVSSettingItemValueType::LastConfirmed);
}