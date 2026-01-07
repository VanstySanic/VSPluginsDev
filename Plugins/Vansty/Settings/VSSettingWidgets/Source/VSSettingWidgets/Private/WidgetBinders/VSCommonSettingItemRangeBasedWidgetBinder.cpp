// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSCommonSettingItemRangeBasedWidgetBinder.h"
#include "VSWidgetController.h"
#include "Items/VSCommonSettingItem.h"

UVSCommonSettingItemRangeBasedWidgetBinder::UVSCommonSettingItemRangeBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonSettingItemRangeBasedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		SettingItem->OnUpdated.AddDynamic(this, &UVSCommonSettingItemRangeBasedWidgetBinder::OnSettingItemUpdated);
	}
}

void UVSCommonSettingItemRangeBasedWidgetBinder::Uninitialize_Implementation()
{
	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		SettingItem->OnUpdated.RemoveDynamic(this, &UVSCommonSettingItemRangeBasedWidgetBinder::OnSettingItemUpdated);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnSettingItemUpdated(UVSSettingItem* SettingItem)
{
	if (UVSWidgetController* WidgetController = GetWidgetController())
	{
		WidgetController->RebindWidgetByType(FName("Value"));
	}
	OnCommonSettingItemUpdated(CommonSettingItemPrivate.Get());
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnCommonSettingItemUpdated_Implementation(UVSCommonSettingItem* SettingItem)
{

}
