// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/VSCommonSettingItemOptionBasedWidgetBinder.h"
#include "CommonRotator.h"
#include "Components/ComboBoxString.h"
#include "Items/VSCommonSettingItem.h"
#include "Mediator/VSSettingItemWidgetController.h"

UVSCommonSettingItemOptionBasedWidgetBinder::UVSCommonSettingItemOptionBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonSettingItemOptionBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(Widget))
		{
			GetWidgetController()->BindWidget(ComboBoxString, FName("Options"));
		}
		else if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(Widget))
		{
			GetWidgetController()->BindWidget(CommonRotator, FName("Options"));
		}
	}
}

void UVSCommonSettingItemOptionBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(Widget))
		{
			GetWidgetController()->UnbindWidget(ComboBoxString, FName("Options"));
		}
		else if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(Widget))
		{
			GetWidgetController()->UnbindWidget(CommonRotator, FName("Options"));
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

FString UVSCommonSettingItemOptionBasedWidgetBinder::GetExternalOption_Implementation() const
{
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		FString Str = CommonSettingItem->GetStringValue();
		return CommonSettingItem->GetStringValue();
	}
	
	return Super::GetExternalOption_Implementation();
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnWidgetOptionChanged_Implementation(int32 NewIndex)
{
	Super::OnWidgetOptionChanged_Implementation(NewIndex);
	
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		CommonSettingItem->SetStringValue(GetOptionAtIndex(NewIndex));
	}
}

FText UVSCommonSettingItemOptionBasedWidgetBinder::OptionStringToText_Implementation(const FString& String) const
{
	if (UVSCommonSettingItem* CommonSettingItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
	{
		return CommonSettingItem->ValueStringToText(String);
	}
	
	return Super::OptionStringToText_Implementation(String);
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindTypedWidget(FName("Options"));
}

