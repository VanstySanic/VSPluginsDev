// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSRangeBasedWidgetBinder.h"
#include "VSCommonSettingItemRangeBasedWidgetBinder.generated.h"

class UVSSettingItem;
class UVSCommonSettingItem;

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemRangeBasedWidgetBinder : public UVSRangeBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSWidgetBinder Interface
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	UVSCommonSettingItem* GetCommonSettingItem() const { return CommonSettingItemPrivate.Get(); }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnCommonSettingItemUpdated(UVSCommonSettingItem* SettingItem);
	
private:
	UFUNCTION()
	void OnSettingItemUpdated(UVSSettingItem* SettingItem);

private:
	TWeakObjectPtr<UVSCommonSettingItem> CommonSettingItemPrivate;
};
