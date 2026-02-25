// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSContentedInputKeySelector.generated.h"

class UInputKeySelector;
class UVSKeyIconConfig;

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSContentedInputKeySelector : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeySelectedDelegate, UVSContentedInputKeySelector* /** Selector */, FInputChord /** SelectedKey */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKeySelectedEvent, UVSContentedInputKeySelector*, Selector, FInputChord, SelectedKey);

public:
	//~ Begin UUserWidget Interface 
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface

	UFUNCTION(BlueprintCallable, Category = "Key Selector")
	UInputKeySelector* GetInputKeySelector() const { return InputKeySelector; }

	UFUNCTION(BlueprintCallable, Category = "Key Selector", meta = (AutoCreateRefTerm = "Key"))
	void SetSelectedKey(const FInputChord& Key);

	UFUNCTION(BlueprintCallable, Category = "Key Selector")
	FInputChord GetSelectedKey() const;
	
	UFUNCTION(BlueprintCallable, Category = "Key  Selector")
	void RefreshContents();

private:
	bool AddKeyIcon(const FKey& Key);
	void AddKeyText(const FKey& Key);
	void AddCustomText(const FText Text);
	
	UFUNCTION()
	void OnIsSelectingChanged();
	
	UFUNCTION()
	void OnSelectorKeySelected(FInputChord SelectedKey);

public:
	FOnKeySelectedDelegate OnKeySelected_Native;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FOnKeySelectedEvent OnKeySelected;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInputKeySelector> InputKeySelector;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Content;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	TObjectPtr<UVSKeyIconConfig> KeyIconConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	FSlateBrush KeyBrush;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	FVSCommonPanelSlotSettings ContentSlotSettings;
};
