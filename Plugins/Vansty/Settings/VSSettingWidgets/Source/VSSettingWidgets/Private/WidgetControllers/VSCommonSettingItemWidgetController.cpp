// Copyright VanstySanic. All Rights Reserved.

#include "WidgetControllers/VSCommonSettingItemWidgetController.h"
#include "Items/VSCommonSettingItem.h"

UVSCommonSettingItemWidgetController::UVSCommonSettingItemWidgetController(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	
}

UVSCommonSettingItem* UVSCommonSettingItemWidgetController::GetCommonSettingItem() const
{
	return CommonSettingItemPrivate.Get();
}

void UVSCommonSettingItemWidgetController::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	CommonSettingItemPrivate = Cast<UVSCommonSettingItem>(GetSettingItem());
}

void UVSCommonSettingItemWidgetController::Uninitialize_Implementation()
{
	CommonSettingItemPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

void UVSCommonSettingItemWidgetController::OnSettingItemUpdated_Implementation(UVSSettingItem* SettingItem)
{
	Super::OnSettingItemUpdated_Implementation(SettingItem);

	OnCommonSettingItemUpdated(CommonSettingItemPrivate.Get());
}

void UVSCommonSettingItemWidgetController::OnCommonSettingItemUpdated_Implementation(UVSCommonSettingItem* SettingItem)
{
	
}
