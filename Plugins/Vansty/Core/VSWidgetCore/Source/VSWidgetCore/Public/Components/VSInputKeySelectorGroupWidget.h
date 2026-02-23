// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSInputKeySelectorGroupWidget.generated.h"

class UInputKeySelector;
/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSInputKeySelectorGroupWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	/** Rebuilds all key selectors based on the current settings. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	void RefreshKeySelectors();
	
public:
	/** Number of keys to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	int32 KeyNum = 1;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSCommonPanelSlotSettings KeySlotSettings;
	
	/**
	 * If false, a refreshment of images will be done during pre-construction.
	 * If true, image refreshment should be executed manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	uint8 bDifferRefreshment : 1;

protected:
	/** The panel widget that contains the key selectors. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Keys;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UInputKeySelector>> KeySelectorsPrivate;

	int32 CurrentSelectedIndex = 0;
};
