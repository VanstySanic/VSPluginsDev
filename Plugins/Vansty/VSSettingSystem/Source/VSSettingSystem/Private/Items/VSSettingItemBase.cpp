// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystem/Public/Items/VSSettingItemBase.h"
#include "VSSettingSubsystem.h"

UVSSettingItemBase::UVSSettingItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryObjectTick.bTickCrossWorldIfPossible = false;
}

void UVSSettingItemBase::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ExecuteActions(
		TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::SetToCurrent,
			EVSSettingItemAction::Load,
			EVSSettingItemAction::Validate,
			EVSSettingItemAction::Apply,
			EVSSettingItemAction::Confirm,
		});
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
		SetToDefault();
		break;
		
	case EVSSettingItemAction::SetToCurrent:
		SetToCurrent();
		break;
		
	case EVSSettingItemAction::Validate:
		Validate();
		break;
		
	case EVSSettingItemAction::Apply:
		Apply();
		break;
		
	case EVSSettingItemAction::Confirm:
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

void UVSSettingItemBase::SetToDefault_Implementation()
{
	
}

void UVSSettingItemBase::SetToCurrent_Implementation()
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

void UVSSettingItemBase::Save_Implementation()
{
}

void UVSSettingItemBase::Confirm_Implementation()
{
}

bool UVSSettingItemBase::IsDirty_Implementation() const
{
	return false;
}