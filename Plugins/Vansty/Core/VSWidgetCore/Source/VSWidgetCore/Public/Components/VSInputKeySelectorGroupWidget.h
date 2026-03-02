// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Types/VSWidgetSettingTypes.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSInputKeySelectorGroupWidget.generated.h"

class UVSContentedInputKeySelector;
class UButton;
class UVSInputKeySelector;
class UVSKeyIconConfig;

/**
 * Widget that manages multiple key selectors as one key-list editor.
 */
UCLASS()
class VSWIDGETCORE_API UVSInputKeySelectorGroupWidget : public UCommonButtonBase
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnKeySelectedDelegate, UVSInputKeySelectorGroupWidget* /** Group */, int32 /** Index */, FInputChord /** SelectedKey */);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeysUpdatedDelegate, UVSInputKeySelectorGroupWidget* /** Group */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKeySelectedEvent, UVSInputKeySelectorGroupWidget*, Group, int32, Index, FInputChord, SelectedKey);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeysUpdatedEvent, UVSInputKeySelectorGroupWidget*, Group);

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	TArray<UVSContentedInputKeySelector*> GetKeySelectors() const { return KeySelectorsPrivate; }

	/** Returns key selector at index, or nullptr when out of range. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	UVSContentedInputKeySelector* GetKeySelectorAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	TArray<FInputChord> GetKeys() const { return CurrentKeys; }

	/** Returns key at index, or empty chord when out of range. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	FInputChord GetKeyAtIndex(int32 Index = 0) const;
	
	/** Sets keys in order; extra keys beyond generated selector count are only stored in `CurrentKeys`. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group", meta = (AutoCreateRefTerm = "InKeys"))
	void SetKeys(const TArray<FInputChord>& InKeys);
	
	/** Sets key at index and notifies events when changed. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	void SetKeyAtIndex(int32 Index = 0, const FInputChord& InKey = FInputChord());
	
	/** Rebuilds all key selectors based on the current settings. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	void RefreshKeySelectors();

private:
	void NotifyKeySelected(int32 Index, FInputChord SelectedKey);
	void NotifyKeysUpdated();

	UFUNCTION()
	void OnSelectorKeySelected(UVSContentedInputKeySelector* Selector, FInputChord SelectedKey);
	
public:
	/** Broadcast when one selector key changes. */
	FOnKeySelectedDelegate OnKeySelected_Native;
	/** Broadcast when key list changes. */
	FOnKeysUpdatedDelegate OnKeysUpdated_Native;
	
	/** Broadcast when one selector key changes. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FOnKeySelectedEvent OnKeySelected;
	
	/** Broadcast when key list changes. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnKeysUpdatedEvent OnKeysUpdated;

public:
	/** Number of keys to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	int32 KeyNum = 1;
	
	/** Key selector class used when generating selector widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	TSubclassOf<UVSContentedInputKeySelector> InputKeySelectorClass;

	/** Key filtering settings applied to generated selectors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSInputKeySelectorKeySettings KeySettings;

	/** Slot settings applied to generated selectors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSCommonPanelSlotSettings KeySlotSettings;

	/** Style settings applied to generated selectors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSInputKeySelectorStyleSettings StyleSettings;
	
	/** Optional icon config propagated to generated selectors when set. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Selector Group")
	TObjectPtr<UVSKeyIconConfig> KeyIconConfig;

	/** Applies only when `KeyBrush.DrawAs` is not `NoDrawType`. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Selector Group")
	FSlateBrush KeyBrush;
	
	/** If true, skips runtime auto-refresh in `NativePreConstruct` and expects manual `RefreshKeySelectors`. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	uint8 bDifferRefreshment : 1;

protected:
	/** The panel widget that contains the key selectors. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Keys;


private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Keys", Category = "Key Selector Group")
	TArray<FInputChord> EditorPreviewKeys;
#endif
	
	UPROPERTY()
	TArray<TObjectPtr<UVSContentedInputKeySelector>> KeySelectorsPrivate;

	/** Backing key list synchronized with child selectors. */
	TArray<FInputChord> CurrentKeys;
};
