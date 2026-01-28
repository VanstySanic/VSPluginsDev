// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSSettingItemMutableRangedWidgetBinder.h"

#include "VSSettingSubsystem.h"
#include "VSWidgetController.h"
#include "Components/VSCommonRanger.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSMutableFloatSettingItem.h"

UVSSettingItemMutableRangedWidgetBinder::UVSSettingItemMutableRangedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSSettingItemMutableRangedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();
}

void UVSSettingItemMutableRangedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			GetWidgetController()->BindWidget(CommonRanger, FName("Range"));
		}
	}
}

void UVSSettingItemMutableRangedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			GetWidgetController()->UnbindWidget(CommonRanger, FName("Range"));
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

void UVSSettingItemMutableRangedWidgetBinder::OnWidgetValueChanged_Implementation(float NewValue)
{
	Super::OnWidgetValueChanged_Implementation(NewValue);
	
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		CommonSettingItem->SetFloatValue(GetCurrentValue());
	}
	else if (UVSMutableFloatSettingItem* MutableFloatSettingItem = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		MutableFloatSettingItem->SetValue(GetCurrentNonMutedValue());
		MutableFloatSettingItem->SetIsMuted(GetCurrentIsMuted());
	}
	
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Content"));
}

void UVSSettingItemMutableRangedWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Content"));
}

#if WITH_EDITOR
void UVSSettingItemMutableRangedWidgetBinder::EditorRefreshMediator_Implementation()
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
