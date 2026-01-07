// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSOptionBasedWidgetBinder.h"
#include "VSCommonSettingItemOptionBasedWidgetBinder.generated.h"

class UVSSettingItem;
class UVSCommonSettingItem;

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemOptionBasedWidgetBinder : public UVSOptionBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSWidgetBinder Interface


	//~ Begin UVSOptionBasedWidgetBinder Interface
	virtual FString GetCurrentOptionKey_Implementation() const override;
	virtual void OnWidgetOptionChanged_Implementation(const FString& NewKey, const FText& NewText, int32 NewIndex) override;
	//~ End UVSOptionBasedWidgetBinder Interface
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	UVSCommonSettingItem* GetCommonSettingItem() const { return CommonSettingItemPrivate.Get(); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Text"))
	FString ItemTextToString(const FText& Text) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "String"))
	FText ItemStringToText(const FString& String) const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnCommonSettingItemUpdated(UVSCommonSettingItem* SettingItem);
	
private:
	UFUNCTION()
	void OnSettingItemUpdated(UVSSettingItem* SettingItem);

private:
	TWeakObjectPtr<UVSCommonSettingItem> CommonSettingItemPrivate;
};
