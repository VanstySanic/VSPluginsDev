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
	//~ Begin UVSOptionBasedWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	
	virtual FString GetExternalOption_Implementation() const override;
	virtual FText OptionStringToText_Implementation(const FString& String) const override;
	//~ End UVSOptionBasedWidgetBinder Interface
	
protected:
	virtual void OnWidgetOptionChanged_Implementation(int32 NewIndex) override;

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
