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
	//~ Begin UVSRangeBasedWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	
	virtual void OnWidgetValueChanged_Implementation(float NewValue) override;
	
	//~ End UVSRangeBasedWidgetBinder Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	UVSCommonSettingItem* GetCommonSettingItem() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnCommonSettingItemUpdated(UVSCommonSettingItem* SettingItem);
	
private:
	UFUNCTION()
	void OnSettingItemUpdated(UVSSettingItem* SettingItem);

private:
	TWeakObjectPtr<UVSCommonSettingItem> CommonSettingItemPrivate;
};
