// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/VSSettingItemMutableRangedWidgetBinder.h"
#include "VSSettingSystemUtils.h"
#include "Mediator/VSWidgetController.h"
#include "Components/VSCommonRanger.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSMutableFloatSettingItem.h"

UVSSettingItemMutableRangedWidgetBinder::UVSSettingItemMutableRangedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
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
		CommonSettingItem->SetFloatValue(GetWidgetValue());
	}
	else if (UVSMutableFloatSettingItem* MutableFloatSettingItem = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		MutableFloatSettingItem->SetValue(GetWidgetNonMutedValue());
		MutableFloatSettingItem->SetIsMuted(GetWidgetIsMuted());
	}
	
	RebindTypedWidget(FName("Range"));
	RebindTypedWidget(FName("Content"));
}

void UVSSettingItemMutableRangedWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindTypedWidget(FName("Range"));
	RebindTypedWidget(FName("Content"));
}

float UVSSettingItemMutableRangedWidgetBinder::GetExternalNonMutedValue_Implementation() const
{
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		return CommonSettingItem->GetFloatValue(EVSSettingItemValueSource::System);
	}
	if (UVSMutableFloatSettingItem* MutableRanger = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		return MutableRanger->GetNonMutedValue(EVSSettingItemValueSource::System);
	}
	
	return Super::GetExternalNonMutedValue_Implementation();
}

bool UVSSettingItemMutableRangedWidgetBinder::GetExternalIsMuted_Implementation() const
{
	if (!GetSettingItem_Native()) return false;
	
	if (UVSMutableFloatSettingItem* MutableRanger = Cast<UVSMutableFloatSettingItem>(GetSettingItem_Native()))
	{
		return MutableRanger->GetIsMuted(EVSSettingItemValueSource::System);
	}
	
	return Super::GetExternalIsMuted_Implementation();
}

#if WITH_EDITOR
void UVSSettingItemMutableRangedWidgetBinder::EditorRefreshMediator_Implementation()
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
			DisplayMutedText = MutableRanger->DisplayMutedText;
		}
	}
}
#endif
