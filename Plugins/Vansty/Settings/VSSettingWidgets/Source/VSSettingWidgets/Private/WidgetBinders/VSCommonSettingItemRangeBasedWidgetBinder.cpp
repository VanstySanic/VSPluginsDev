// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSCommonSettingItemRangeBasedWidgetBinder.h"

#include "VSSettingSubsystem.h"
#include "VSWidgetController.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/VSCommonMutableRanger.h"
#include "Components/VSCommonRanger.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSMutableFloatSettingItem.h"
#include "WidgetControllers/VSSettingItemWidgetController.h"

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
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Content"));
}

#if WITH_EDITOR
void UVSCommonSettingItemRangeBasedWidgetBinder::EditorRefreshMediator_Implementation()
{
	if (!GetSettingItem_Native()) return;
	const FGameplayTag& ItemTag = GetSettingItem_Native()->GetItemTag();
	
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