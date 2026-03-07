// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/VSCommonSettingItemRangeBasedWidgetBinder.h"
#include "VSSettingSystemUtils.h"
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
		CommonSettingItem->SetFloatValue(GetWidgetValue());
	}
	if (UVSMutableFloatSettingItem* MutableFloatSettingItem = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		MutableFloatSettingItem->SetValue(GetWidgetValue());
	}
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindTypedWidget(FName("Range"));
	RebindTypedWidget(FName("Content"));
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
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();
	
	if (UVSSettingSystemUtils::GetSettingSubsystemVS())
	{
		if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(UVSSettingSystemUtils::GetSettingItemByIdentifier(ItemIdentifier)))
		{
			DisplayTextFormat = CommonSettingItem->DisplayNumericTextFormat;
			DisplayFractionDigitRange = CommonSettingItem->DisplayNumericFractionDigitRange;
			DisplayValueMultiplier = CommonSettingItem->DisplayNumericValueMultiplier;
		}
		else if (UVSMutableFloatSettingItem* MutableRanger = Cast<UVSMutableFloatSettingItem>(UVSSettingSystemUtils::GetSettingItemByIdentifier(ItemIdentifier)))
		{
			DisplayTextFormat = MutableRanger->DisplayTextFormat;
			DisplayFractionDigitRange = MutableRanger->DisplayFractionDigitRange;
			DisplayValueMultiplier = MutableRanger->DisplayValueMultiplier;
		}
	}
}
#endif
