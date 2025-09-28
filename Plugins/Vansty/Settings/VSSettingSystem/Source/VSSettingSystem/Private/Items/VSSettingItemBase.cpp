// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystem/Public/Items/VSSettingItemBase.h"
#include "VSSettingSubsystem.h"
#include "Components/ComboBoxKey.h"
#include "Components/ComboBoxString.h"
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

	OnCultureChangedDelegateHandle = FInternationalization::Get().OnCultureChanged().AddUObject(this, &ThisClass::OnCultureChanged);
	OnWorldBeginTearDownDelegateHandle = FWorldDelegates::OnWorldBeginTearDown.AddLambda([&] (UWorld*)
	{
		const TMultiMap<TWeakObjectPtr<UWidget>, FName> CopiedTypedBoundWidgetMap;
		for (const TPair<TWeakObjectPtr<UWidget>, FName>& BoundWidgetMap : CopiedTypedBoundWidgetMap)
		{
			if (BoundWidgetMap.Key.IsValid())
			{
				UnbindWidget(BoundWidgetMap.Key.Get(), BoundWidgetMap.Value);
			}
		}

		TypedBoundWidgetMap.Empty();
		TypedBoundWidgetMultimap.Empty();
	});
}

void UVSSettingItemBase::Uninitialize_Implementation()
{
	check(bHasBeenInitialized);
	bHasBeenInitialized = false;

	if (OnWorldBeginTearDownDelegateHandle.IsValid())
	{
		FWorldDelegates::OnWorldBeginTearDown.Remove(OnWorldBeginTearDownDelegateHandle);
	}
	if (OnCultureChangedDelegateHandle.IsValid())
	{
		FInternationalization::Get().OnCultureChanged().Remove(OnCultureChangedDelegateHandle);
	}

	const TMultiMap<TWeakObjectPtr<UWidget>, FName> CopiedTypedBoundWidgetMap;
	for (const TPair<TWeakObjectPtr<UWidget>, FName>& BoundWidgetMap : CopiedTypedBoundWidgetMap)
	{
		if (BoundWidgetMap.Key.IsValid())
		{
			UnbindWidget(BoundWidgetMap.Key.Get(), BoundWidgetMap.Value);
		}
	}
}

#if WITH_EDITOR
void UVSSettingItemBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase, ItemInfo))
	{
		TArray<TWeakObjectPtr<UWidget>> Widgets;
		TypedBoundWidgetMap.GetKeys(Widgets);
		for (TWeakObjectPtr<UWidget> BoundWidget : Widgets)
		{
			RebindWidget(BoundWidget.Get());
		}
	}
	
	UObject::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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
		break;
		
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
}

void UVSSettingItemBase::ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions)
{
	for (const TEnumAsByte<EVSSettingItemAction::Type>& Action : Actions)
	{
		ExecuteAction(Action);
	}
}

void UVSSettingItemBase::NotifyValueUpdate()
{
	OnItemValueUpdated();
	OnValueUpdated.Broadcast();
	if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
	{
		SettingSubsystem->OnSettingItemUpdated.Broadcast(this);
	}
	ExecuteActions(ValueUpdatedActions);
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

void UVSSettingItemBase::BindWidget(UWidget* Widget, FName TypeName)
{
	if (!Widget || !IsValid(Widget)) return;

	if (TypedBoundWidgetMap.Contains(Widget))
	{
		UnbindWidget(Widget, TypedBoundWidgetMap.FindRef(Widget));
	}

	TypedBoundWidgetMap.Emplace(Widget, TypeName);
	TypedBoundWidgetMultimap.Add(TypeName, Widget);

	BindWidgetInternal(Widget, TypeName);
}

void UVSSettingItemBase::RebindWidget(UWidget* Widget)
{
	const FName TypeName = GetWidgetBoundTypeName(Widget);
	UnbindWidget(Widget, TypeName);
	BindWidget(Widget, TypeName);
}

void UVSSettingItemBase::UnbindWidget(UWidget* Widget, FName TypeName)
{
	if (!TypedBoundWidgetMap.Contains(Widget)) return;
	if (!TypedBoundWidgetMultimap.Contains(TypeName)) return;
	
	TArray<TWeakObjectPtr<UWidget>> FoundWidgets;
	TypedBoundWidgetMultimap.MultiFind(TypeName, FoundWidgets);
	if (!FoundWidgets.Contains(Widget)) return;

	TypedBoundWidgetMap.Remove(Widget);
	TypedBoundWidgetMultimap.Remove(TypeName, Widget);

	UnbindWidgetInternal(Widget, TypeName);
}

bool UVSSettingItemBase::IsWidgetBound(UWidget* Widget) const
{
	return TypedBoundWidgetMap.Contains(Widget);
}

FName UVSSettingItemBase::GetWidgetBoundTypeName(UWidget* Widget) const
{
	return IsWidgetBound(Widget) ? TypedBoundWidgetMap.FindRef(Widget) : NAME_None;
}

void UVSSettingItemBase::RebindAllWidgets()
{
	const TMultiMap<TWeakObjectPtr<UWidget>, FName>& CopiedTypedBoundWidgetMap = TypedBoundWidgetMap;
	for (const TPair<TWeakObjectPtr<UWidget>, FName>& BoundWidgetMap : CopiedTypedBoundWidgetMap)
	{
		RebindWidget(BoundWidgetMap.Key.Get());
	}
}

void UVSSettingItemBase::RebindWidgetsOfType(FName TypeName)
{
	const TArray<UWidget*>& TypedWidgets = GetBoundWidgetsOfType(TypeName);
	for (UWidget* TypedWidget : TypedWidgets)
	{
		RebindWidget(TypedWidget);
	}
}

TArray<UWidget*> UVSSettingItemBase::GetBoundWidgetsOfType(FName TypeName) const
{
	if (!TypedBoundWidgetMultimap.Contains(TypeName)) return TArray<UWidget*>();
	TArray<TWeakObjectPtr<UWidget>> FoundWidgets;
	TypedBoundWidgetMultimap.MultiFind(TypeName, FoundWidgets);

	TArray<UWidget*> Widgets;
	for (TWeakObjectPtr<UWidget> FoundWidget : FoundWidgets)
	{
		if (FoundWidget.IsValid())
		{
			Widgets.Add(FoundWidget.Get());
		}
	}

	return Widgets;
}

void UVSSettingItemBase::OnCultureChanged_Implementation()
{
	RebindWidgetsOfType(FName("Value"));
	RebindWidgetsOfType(FName("Content"));
}

void UVSSettingItemBase::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Name"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(ItemInfo.DisplayName);
		}
	}
}

void UVSSettingItemBase::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
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

void UVSSettingItemBase::OnItemValueUpdated_Implementation()
{
	
}
