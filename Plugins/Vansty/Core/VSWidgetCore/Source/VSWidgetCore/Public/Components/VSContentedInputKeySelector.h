// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSContentedInputKeySelector.generated.h"

class UInputKeySelector;
class UVSKeyIconConfig;

/**
 * Composite key selector widget that renders selected key as icon/text
 * content while reusing an internal InputKeySelector for interaction.
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

	/** Sets selected key on internal key selector. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector", meta = (AutoCreateRefTerm = "Key"))
	void SetSelectedKey(const FInputChord& Key);

	/** Returns selected key from internal key selector. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector")
	FInputChord GetSelectedKey() const;
	
	/** Rebuilds icon/text content according to current selected key. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector")
	void RefreshContents();

private:
	/** Adds key icon content when icon exists in `KeyIconConfig`. */
	bool AddKeyIcon(const FKey& Key);
	/** Adds key display-name text content. */
	void AddKeyText(const FKey& Key);
	void AddCustomText(const FText Text);
	
	UFUNCTION()
	void OnIsSelectingChanged();
	
	UFUNCTION()
	void OnSelectorKeySelected(FInputChord SelectedKey);

public:
	/** Broadcast when key is selected. */
	FOnKeySelectedDelegate OnKeySelected_Native;

	/** Broadcast when key is selected. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FOnKeySelectedEvent OnKeySelected;
	
protected:
	/** Internal key selector that handles key capture and selection state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInputKeySelector> InputKeySelector;

	/** Content panel used to render icon/text representation of selected key. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Content;

public:
	/** Optional key-icon config used to resolve key textures. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	TObjectPtr<UVSKeyIconConfig> KeyIconConfig;

	/** Base brush used when drawing key icons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	FSlateBrush KeyBrush;
	
	/** Slot settings applied to generated content widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector")
	FVSCommonPanelSlotSettings ContentSlotSettings;
};
