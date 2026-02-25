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
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSInputKeySelectorGroupWidget : public UCommonButtonBase
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnKeySelectedDelegate, UVSInputKeySelectorGroupWidget* /** Group */, int32 /** IndexIndex */, FInputChord /** SelectedKey */);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeysUpdatedDelegate, UVSInputKeySelectorGroupWidget* /** Group */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKeySelectedEvent, UVSInputKeySelectorGroupWidget*, Group, int32, Index, FInputChord, SelectedKey);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeysUpdatedEvent, UVSInputKeySelectorGroupWidget*, Group);

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	TArray<UVSContentedInputKeySelector*> GetKeySelectors() const { return KeySelectorsPrivate; }

	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	UVSContentedInputKeySelector* GetKeySelectorAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	TArray<FInputChord> GetKeys() const { return CurrentKeys; }

	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	FInputChord GetKeyAtIndex(int32 Index = 0) const;
	
	/** Set keys in order. If the passed-in key num is greater than widget key num, then only part of the keys will be set. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group", meta = (AutoCreateRefTerm = "InKeys"))
	void SetKeys(const TArray<FInputChord>& InKeys);
	
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
	FOnKeySelectedDelegate OnKeySelected_Native;
	FOnKeysUpdatedDelegate OnKeysUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FOnKeySelectedEvent OnKeySelected;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnKeysUpdatedEvent OnKeysUpdated;

public:
	/** Number of keys to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	int32 KeyNum = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	TSubclassOf<UVSContentedInputKeySelector> InputKeySelectorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSInputKeySelectorKeySettings KeySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSCommonPanelSlotSettings KeySlotSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSInputKeySelectorStyleSettings StyleSettings;
	
	/** Apply if not null. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Selector Group")
	TObjectPtr<UVSKeyIconConfig> KeyIconConfig;

	/** Apply if KeyBrush.DrawAs != ESlateBrushDrawType::NoDrawType */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Selector Group")
	FSlateBrush KeyBrush;
	
	/**
	 * If false, a refreshment of widgets will be done during pre-construction.
	 * If true, refreshment should be executed manually.
	 */
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

	TArray<FInputChord> CurrentKeys;
};
