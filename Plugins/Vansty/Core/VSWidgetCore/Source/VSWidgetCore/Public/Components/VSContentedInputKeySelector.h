// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSContentedInputKeySelector.generated.h"

class UVSInputKeySelector;
class UVSKeyIconConfig;

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSContentedInputKeySelector : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UUserWidget Interface 
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface

	UFUNCTION(BlueprintCallable, Category = "Key  Selector")
	UVSInputKeySelector* GetInputKeySelector() const { return InputKeySelector; }
	
	UFUNCTION(BlueprintCallable, Category = "Key  Selector")
	void RefreshContents();

private:
	bool AddKeyIcon(const FKey& Key);
	void AddKeyText(const FKey& Key);
	void AddCustomText(const FText Text);
	
	UFUNCTION()
	void OnIsSelectingChanged();
	
	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVSInputKeySelector> InputKeySelector;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Content;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	TObjectPtr<UVSKeyIconConfig> KeyIconConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	FSlateBrush KeyBrush;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	FVSCommonPanelSlotSettings ContentPanelSlotSettings;
};
