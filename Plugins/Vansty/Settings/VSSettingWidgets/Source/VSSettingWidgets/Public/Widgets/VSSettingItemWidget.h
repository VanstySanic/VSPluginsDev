// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Components/TextBlock.h"
#include "Types/VSWidgetSlotTypes.h"
#include "WidgetBinders/VSDesiredBoundWidgetInterface.h"
#include "VSSettingItemWidget.generated.h"

class UVSSettingItemWidgetController;

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemWidget : public UCommonButtonBase, public IVSDesiredBoundWidgetInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	//~ Begin UVSWidgetBinder Interface
	virtual TMap<UWidget*, FName> GetDesiredBoundTypedWidgets_Implementation() const override;
	//~ End UVSWidgetBinder Interface
	
private:
	void RefreshCoreWidget();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UTextBlock> TextBlock_Name;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UPanelWidget> Panel_CoreWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UWidget> CoreWidget;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UWidget> CoreWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Settings")
	TObjectPtr<UVSSettingItemWidgetController> SettingWidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVSCommonPanelSlotSettings CoreWidgetPanelSlotSettings;

};
