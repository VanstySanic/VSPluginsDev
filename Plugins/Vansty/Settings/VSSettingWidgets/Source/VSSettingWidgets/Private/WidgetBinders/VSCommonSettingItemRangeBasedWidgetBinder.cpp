// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSCommonSettingItemRangeBasedWidgetBinder.h"
#include "VSWidgetController.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/VSCommonRanger.h"
#include "Items/VSCommonSettingItem.h"
#include "WidgetControllers/VSSettingItemWidgetController.h"

UVSCommonSettingItemRangeBasedWidgetBinder::UVSCommonSettingItemRangeBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonSettingItemRangeBasedWidgetBinder::Initialize_Implementation()
{
	CommonSettingItemPrivate = GetCommonSettingItem();
	
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

void UVSCommonSettingItemRangeBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Core"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			BindTypedWidget(FName("Range"), Slider);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			BindTypedWidget(FName("Range"), SpinBox);
		}
		else if (UVSCommonRanger* Ranger = Cast<UVSCommonRanger>(Widget))
		{
			BindTypedWidget(FName("Range"), Ranger);
		}
	}
}

void UVSCommonSettingItemRangeBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Core"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			UnbindTypedWidget(FName("Range"), Slider);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			UnbindTypedWidget(FName("Range"), SpinBox);
		}
		else if (UVSCommonRanger* Ranger = Cast<UVSCommonRanger>(Widget))
		{
			UnbindTypedWidget(FName("Range"), Ranger);
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnWidgetValueChanged_Implementation(float NewValue)
{
	Super::OnWidgetValueChanged_Implementation(NewValue);
	
	if (CommonSettingItemPrivate.IsValid())
	{
		CommonSettingItemPrivate->SetFloatValue(NewValue);
	}
}

UVSCommonSettingItem* UVSCommonSettingItemRangeBasedWidgetBinder::GetCommonSettingItem() const
{
	if (CommonSettingItemPrivate.IsValid()) return CommonSettingItemPrivate.Get();

	if (UVSSettingItemWidgetController* WidgetController = Cast<UVSSettingItemWidgetController>(GetWidgetController()))
	{
		if (UVSCommonSettingItem* SettingItem = Cast<UVSCommonSettingItem>(WidgetController->GetSettingItem()))
		{
			return SettingItem;
		}
	}

	return nullptr;
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnSettingItemUpdated(UVSSettingItem* SettingItem)
{
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Content"));
	OnCommonSettingItemUpdated(CommonSettingItemPrivate.Get());
}

void UVSCommonSettingItemRangeBasedWidgetBinder::OnCommonSettingItemUpdated_Implementation(UVSCommonSettingItem* SettingItem)
{

}