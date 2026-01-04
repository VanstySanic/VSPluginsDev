// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemWidgetController.h"
#include "VSCommonSettingItemWidgetController.generated.h"

class UVSCommonSettingItem;

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemWidgetController : public UVSSettingItemWidgetController
{
	GENERATED_UCLASS_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	UVSCommonSettingItem* GetCommonSettingItem() const;
	
protected:
	//~ Begin UVSWidgetController Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSWidgetController Interface

	//~ Begin UVSSettingItemWidgetController Interface
	virtual void OnSettingItemUpdated_Implementation(UVSSettingItem* SettingItem) override;
	//~ End UVSSettingItemWidgetController Interface
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnCommonSettingItemUpdated(UVSCommonSettingItem* SettingItem);

private:
	TWeakObjectPtr<UVSCommonSettingItem> CommonSettingItemPrivate;

};
