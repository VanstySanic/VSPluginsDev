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

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeysUpdatedDelegate, UVSInputKeySelectorGroupWidget* /** Group */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeysUpdatedEvent, UVSInputKeySelectorGroupWidget*, Group);

public:
	//~ Begin UUserWidget Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	TArray<UVSContentedInputKeySelector*> GetKeySelectors() const { return KeySelectorsPrivate; }

	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	UVSContentedInputKeySelector* GetKeySelectorAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	TArray<FInputChord> GetKeys() const;
	
	/** Set keys in order. If the passed-in key num is greater than widget key num, then only part of the keys will be set. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group", meta = (AutoCreateRefTerm = "InKeys"))
	void SetKeys(const TArray<FInputChord>& InKeys);
	
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	void SetKeyAtIndex(int32 Index, const FInputChord& InKey);
	
	/** Rebuilds all key selectors based on the current settings. */
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	void RefreshKeySelectors();

private:
	void NotifyKeysUpdated();
	
	UFUNCTION()
	void OnKeySelectionChanged(FInputChord SelectedKey);
	
public:
	FOnKeysUpdatedDelegate OnKeysUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnKeysUpdatedEvent OnKeysUpdated;

public:
	/** Number of keys to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	int32 KeyNum = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	TSubclassOf<UVSContentedInputKeySelector> InputKeySelectorClass;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSCommonPanelSlotSettings KeyPanelSlotSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	FVSInputKeySelectorStyleSettings StyleSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selector Group")
	TArray<FVSInputKeySelectorKeySettings> KeySettings;

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

	int32 CurrentSelectedIndex = 0;
	uint8 bBlockKeySelectionChanged : 1;
};
