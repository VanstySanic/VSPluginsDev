// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSCommonSettingItemOptionBasedWidgetBinder.h"
#include "VSWidgetController.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingItem.h"

UVSCommonSettingItemOptionBasedWidgetBinder::UVSCommonSettingItemOptionBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonSettingItemOptionBasedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		SettingItem->OnUpdated.AddDynamic(this, &UVSCommonSettingItemOptionBasedWidgetBinder::OnSettingItemUpdated);
	}
}

void UVSCommonSettingItemOptionBasedWidgetBinder::Uninitialize_Implementation()
{
	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		SettingItem->OnUpdated.RemoveDynamic(this, &UVSCommonSettingItemOptionBasedWidgetBinder::OnSettingItemUpdated);
	}
	
	Super::Uninitialize_Implementation();
}

FString UVSCommonSettingItemOptionBasedWidgetBinder::GetCurrentOptionKey_Implementation() const
{
	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		return SettingItem->GetStringValue();
	}
	
	return Super::GetCurrentOptionKey_Implementation();
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnWidgetOptionChanged_Implementation(const FString& NewKey, const FText& NewText, int32 NewIndex)
{
	Super::OnWidgetOptionChanged_Implementation(NewKey, NewText, NewIndex);

	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		SettingItem->SetStringValue(NewKey);
	}
}

FString UVSCommonSettingItemOptionBasedWidgetBinder::ItemTextToString_Implementation(const FText& Text) const
{
	return Text.ToString();
}

FText UVSCommonSettingItemOptionBasedWidgetBinder::ItemStringToText_Implementation(const FString& String) const
{
	return FText::FromString(String);
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnSettingItemUpdated(UVSSettingItem* SettingItem)
{
	RebindWidgetByType(FName("Value"));
	OnCommonSettingItemUpdated(CommonSettingItemPrivate.Get());
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnCommonSettingItemUpdated_Implementation(UVSCommonSettingItem* SettingItem)
{

}


