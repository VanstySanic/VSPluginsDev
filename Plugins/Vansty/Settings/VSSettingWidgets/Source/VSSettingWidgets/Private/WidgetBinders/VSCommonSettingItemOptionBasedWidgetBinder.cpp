// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSCommonSettingItemOptionBasedWidgetBinder.h"
#include "CommonRotator.h"
#include "Components/ComboBoxString.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingItem.h"
#include "WidgetControllers/VSSettingItemWidgetController.h"

UVSCommonSettingItemOptionBasedWidgetBinder::UVSCommonSettingItemOptionBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonSettingItemOptionBasedWidgetBinder::Initialize_Implementation()
{
	CommonSettingItemPrivate = GetCommonSettingItem();

	Super::Initialize_Implementation();
	
	if (CommonSettingItemPrivate.IsValid())
	{
		CommonSettingItemPrivate->OnUpdated.AddDynamic(this, &UVSCommonSettingItemOptionBasedWidgetBinder::OnSettingItemUpdated);
	}
}

void UVSCommonSettingItemOptionBasedWidgetBinder::Uninitialize_Implementation()
{
	if (CommonSettingItemPrivate.IsValid())
	{
		CommonSettingItemPrivate->OnUpdated.RemoveDynamic(this, &UVSCommonSettingItemOptionBasedWidgetBinder::OnSettingItemUpdated);
	}
	CommonSettingItemPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

void UVSCommonSettingItemOptionBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Core"))
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
	if (TypeName == FName("Core"))
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
	if (CommonSettingItemPrivate.IsValid())
	{
		FString Str = CommonSettingItemPrivate->GetStringValue();
		UE_LOG(LogTemp,Log,TEXT("Setting item: %s"), *CommonSettingItemPrivate->GetStringValue())
		return CommonSettingItemPrivate->GetStringValue();
	}
	
	return Super::GetExternalOption_Implementation();
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnWidgetOptionChanged_Implementation(int32 NewIndex)
{
	Super::OnWidgetOptionChanged_Implementation(NewIndex);
	
	if (CommonSettingItemPrivate.IsValid())
	{
		CommonSettingItemPrivate->SetStringValue(GetOptionAtIndex(NewIndex));
	}
}

FText UVSCommonSettingItemOptionBasedWidgetBinder::OptionStringToText_Implementation(const FString& String) const
{
	if (UVSCommonSettingItem* SettingItem = GetCommonSettingItem())
	{
		return SettingItem->ValueStringToText(String);
	}
	
	return Super::OptionStringToText_Implementation(String);
}

UVSCommonSettingItem* UVSCommonSettingItemOptionBasedWidgetBinder::GetCommonSettingItem() const
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

void UVSCommonSettingItemOptionBasedWidgetBinder::OnSettingItemUpdated(UVSSettingItem* SettingItem)
{
	RebindWidgetByType(FName("Options"));
	OnCommonSettingItemUpdated(CommonSettingItemPrivate.Get());
}

void UVSCommonSettingItemOptionBasedWidgetBinder::OnCommonSettingItemUpdated_Implementation(UVSCommonSettingItem* SettingItem)
{
	
}


