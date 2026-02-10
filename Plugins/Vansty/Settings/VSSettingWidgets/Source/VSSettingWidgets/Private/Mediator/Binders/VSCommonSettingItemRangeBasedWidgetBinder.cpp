// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/VSCommonSettingItemRangeBasedWidgetBinder.h"
#include "VSSettingSubsystem.h"
#include "Mediator/VSWidgetController.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/VSCommonRanger.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSMutableFloatSettingItem.h"
#include "Mediator/VSSettingItemWidgetController.h"

UVSCommonSettingItemRangeBasedWidgetBinder::UVSCommonSettingItemRangeBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonSettingItemRangeBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			GetWidgetController()->BindWidget(Slider, FName("Range"));
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			GetWidgetController()->BindWidget(SpinBox, FName("Range"));
		}
		else if (UVSCommonRanger* Ranger = Cast<UVSCommonRanger>(Widget))
		{
			GetWidgetController()->BindWidget(Ranger, FName("Range"));
		}
	}
}

void UVSCommonSettingItemRangeBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			GetWidgetController()->UnbindWidget(Slider, FName("Range"));
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			GetWidgetController()->UnbindWidget(SpinBox, FName("Range"));
		}
		else if (UVSCommonRanger* Ranger = Cast<UVSCommonRanger>(Widget))
		{
			GetWidgetController()->UnbindWidget(Ranger, FName("Range"));
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnWidgetValueChanged_Implementation(float NewValue)
{
	Super::OnWidgetValueChanged_Implementation(NewValue);
	
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		CommonSettingItem->SetFloatValue(GetCurrentValue());
	}
	if (UVSMutableFloatSettingItem* MutableFloatSettingItem = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		MutableFloatSettingItem->SetValue(GetCurrentValue());
	}
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Content"));
}

float UVSCommonSettingItemRangeBasedWidgetBinder::GetExternalValue_Implementation() const
{
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		return CommonSettingItem->GetFloatValue(EVSSettingItemValueSource::System);
	}
	if (UVSMutableFloatSettingItem* MutableFloatSettingItem = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		return MutableFloatSettingItem->GetValue(EVSSettingItemValueSource::System);
	}
	
	return Super::GetExternalValue_Implementation();
}

#if WITH_EDITOR
void UVSCommonSettingItemRangeBasedWidgetBinder::EditorRefreshMediator_Implementation()
{
	if (!GetSettingItem_Native()) return;
	const FGameplayTag ItemTag = GetSettingItem_Native()->GetItemTag();
	
	if (UVSSettingSubsystem* Subsystem = UVSSettingSubsystem::Get())
	{
		if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(Subsystem->GetSettingItemByTag(ItemTag)))
		{
			DisplayTextFormat = CommonSettingItem->DisplayNumericTextFormat;
			DisplayFractionDigitRange = CommonSettingItem->DisplayNumericFractionDigitRange;
			DisplayValueMultiplier = CommonSettingItem->DisplayNumericValueMultiplier;
		}
		else if (UVSMutableFloatSettingItem* MutableRanger = Cast<UVSMutableFloatSettingItem>(Subsystem->GetSettingItemByTag(ItemTag)))
		{
			DisplayTextFormat = MutableRanger->DisplayTextFormat;
			DisplayFractionDigitRange = MutableRanger->DisplayFractionDigitRange;
			DisplayValueMultiplier = MutableRanger->DisplayValueMultiplier;
		}
	}
}
#endif
