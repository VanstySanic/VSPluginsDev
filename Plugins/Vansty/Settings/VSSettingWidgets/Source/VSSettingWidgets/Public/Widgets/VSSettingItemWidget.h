// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Components/TextBlock.h"
#include "Types/VSWidgetSlotTypes.h"
#include "Mediator/Binders/VSDesiredBoundWidgetInterface.h"
#include "VSSettingItemWidget.generated.h"

class UVSSettingItemWidgetController;

/**
 * Composite setting entry widget that hosts a title and a dynamic core editor widget.
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
	/** Recreates CoreWidget from CoreWidgetClass and rebinds it to SettingWidgetController. */
	void RefreshCoreWidget();
	
protected:
	/** Optional title text widget, bound as typed widget "Name". */
	UPROPERTY(VisibleAnywhere, Category = "Settings", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Name;

	/** Panel used to host CoreWidget instance. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_CoreWidget;

	/** Runtime-created core editor widget, bound as typed widget "Core". */
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UWidget> CoreWidget;

protected:
	/** Per-item controller that resolves item by tag and coordinates binder updates. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Settings")
	TObjectPtr<UVSSettingItemWidgetController> SettingWidgetController;
	
	/** Core editor widget class instantiated during pre-construct. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UWidget> CoreWidgetClass;

	/** Slot settings applied when CoreWidget is attached to Panel_CoreWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVSCommonPanelSlotSettings CoreWidgetPanelSlotSettings;
};
