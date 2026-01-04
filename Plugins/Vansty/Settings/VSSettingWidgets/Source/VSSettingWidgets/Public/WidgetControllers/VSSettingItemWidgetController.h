// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSWidgetController.h"
#include "UObject/Object.h"
#include "VSSettingItemWidgetController.generated.h"

class UVSSettingItem;

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemWidgetController : public UVSWidgetController
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
	
	//~ Begin UVSWidgetController Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	//~ End UVSWidgetController Interface

	UFUNCTION(BlueprintCallable, Category = "Settings")
	UVSSettingItem* GetSettingItem() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnSettingItemUpdated(UVSSettingItem* SettingItem);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FGameplayTag ItemTag;

private:
	TWeakObjectPtr<UVSSettingItem> SettingItemPrivate;
};
