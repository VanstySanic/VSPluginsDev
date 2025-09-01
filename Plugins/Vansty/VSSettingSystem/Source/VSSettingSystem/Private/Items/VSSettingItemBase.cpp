// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystem/Public/Items/VSSettingItemBase.h"
#include "VSSettingSubsystem.h"
#include "Components/TextBlock.h"

UVSSettingItemBase::UVSSettingItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItemBase::Initialize_Implementation()
{
	check(!bHasBeenInitialized);
	bHasBeenInitialized = true;

	ItemInfo.ItemTags.AddTag(ItemInfo.SpecifyTag);
	ItemInfo.ItemTags.AddTag(ItemInfo.CategoryTag);
}

void UVSSettingItemBase::Uninitialize_Implementation()
{
	check(bHasBeenInitialized);
	bHasBeenInitialized = false;
}

void UVSSettingItemBase::BeginDestroy()
{
	if (bHasBeenInitialized)
	{
		Uninitialize();
	}
	
	UObject::BeginDestroy();
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
		if (EqualsToBySource(EVSSettingItemValueSource::Default)) return;
		SetToBySource(EVSSettingItemValueSource::Default);
		break;
		
	case EVSSettingItemAction::SetToCurrent:
		if (EqualsToBySource(EVSSettingItemValueSource::Current)) return;
		SetToBySource(EVSSettingItemValueSource::Current);
		break;
		
	case EVSSettingItemAction::SetToLastConfirmed:
		if (EqualsToBySource(EVSSettingItemValueSource::LastConfirmed)) return;
		SetToBySource(EVSSettingItemValueSource::LastConfirmed);
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

void UVSSettingItemBase::BindWidget(UWidget* Widget, FName TypeID)
{
	if (TypeID == FName("Name"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(ItemInfo.DisplayName);
		}
	}
}

void UVSSettingItemBase::UnbindWidget(UWidget* Widget, FName TypeID)
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

void UVSSettingItemBase::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	
}

bool UVSSettingItemBase::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return false;
}

bool UVSSettingItemBase::IsDirty() const
{
	return EqualsToBySource(EVSSettingItemValueSource::Settings) != EqualsToBySource(EVSSettingItemValueSource::Current);
}

bool UVSSettingItemBase::IsDefault() const
{
	return EqualsToBySource(EVSSettingItemValueSource::Settings) == EqualsToBySource(EVSSettingItemValueSource::Default);
}

bool UVSSettingItemBase::IsUnconfirmed() const
{
	return EqualsToBySource(EVSSettingItemValueSource::Settings) != EqualsToBySource(EVSSettingItemValueSource::LastConfirmed);
}
