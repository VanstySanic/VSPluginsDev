// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "Types/VSWidgetSettingTypes.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSCommonButtonGroupWidget.generated.h"

class UVSIndexImageGroupWidget;
class UCommonButtonGroupBase;
class UCommonButtonBase;

/**
 * Widget that manages multiple generated buttons as one
 * selectable option group.
 */
UCLASS()
class VSWIDGETCORE_API UVSCommonButtonGroupWidget : public UCommonButtonBase
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRefreshedDelegate, UVSCommonButtonGroupWidget* /** Widget */);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSelectionChangedDelegate, UVSCommonButtonGroupWidget* /** Widget */, int32 /** Index */);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshedEvent, UVSCommonButtonGroupWidget*, Widget);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectionChangedEvent, UVSCommonButtonGroupWidget*, Widget, int32, Index);

protected:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	virtual bool Initialize() override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	//~ End UUserWidget Interface

public:
	/** Regenerates all buttons from current settings. */
	UFUNCTION(BlueprintCallable, Category = "Button Group")
	void RefreshButtons();
	
	/**
	 * Returns internal button group for selection query/binding.
	 * @warning Do not add, remove or modify buttons here.
	 */
	UFUNCTION(BlueprintCallable, Category = "Button Group")
	UCommonButtonGroupBase* GetButtonGroup() const { return ButtonGroupPrivate; }

private:
	/** Handles keyboard/gamepad navigation and updates selected button. */
	TSharedPtr<SWidget> HandleNavigation(EUINavigation UINavigation);
	
	UFUNCTION()
	void OnButtonGroupSelectionChanged(UCommonButtonBase* AssociatedButton, int32 ButtonIndex);
	
	UFUNCTION()
	void OnButtonGroupButtonClicked(UCommonButtonBase* AssociatedButton, int32 ButtonIndex);
	
	UFUNCTION()
	void OnButtonPrevClicked();

	UFUNCTION()
	void OnButtonNextClicked();
	
public:
	/** Broadcast after button regeneration. */
	FOnRefreshedDelegate OnRefreshed_Native;
	/** Broadcast selected index changes. */
	FOnSelectionChangedDelegate OnSelectionChanged_Native;
	
	/** Broadcast after button regeneration. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnRefreshedEvent OnRefreshed;
	
	/** Broadcast when selected index changes. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnSelectionChangedEvent OnSelectionChanged;
	
public:
	/** Button class to generate in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	TSubclassOf<UCommonButtonBase> ButtonClass;

	/** Button count in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	int32 ButtonNum = 1;

	/** Per-index display name override when entry text is non-empty. */
	UPROPERTY(EditAnywhere, Category = "Rotator")
	TArray<FText> OverridenButtonNames;

	/** Indices that should be disabled after regeneration. */
	UPROPERTY(EditAnywhere, Category = "Rotator")
	TArray<int32> DefaultDisabledIndexes;
	
	/** Desired focus target index after refresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group", meta = (EditCondition = "bSupportButtonFocus"))
	int32 DesiredFocusButtonIndex = INDEX_NONE;

	/** Whether the buttons are focusable in the group (reactable by keyboard or gamepad). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bSupportButtonFocus : 1;

	/** If true, allow wrapping in circle when switching buttons by navigation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bNavigationAllowWrapping : 1;

	/** If true, the navigation flow direction will be vertical instead of the default horizontal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bVerticalFlowDirection : 1;
	
	/**
	 * If false, buttons are refreshed in pre-construct.
	 * If true, call RefreshButtons manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bDifferRefreshment : 1;
	
	/** Slot settings applied to generated buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	FVSCommonPanelSlotSettings ButtonSlotSettings;

	/** Style settings applied to generated buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	FVSCommonButtonStyleSettings ButtonStyleSettings;

	/** Per-index action settings for generated buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	TArray<FVSCommonButtonActionSettings> ButtonActionSettings;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button Group", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Prev;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button Group", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Next;
	
	/** The panel widget that contains the buttons. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Button Group")
	TObjectPtr<UPanelWidget> Panel_Buttons;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Index", Category = "Button Group")
	int32 EditorPreviewIndex = INDEX_NONE;
#endif
	
	UPROPERTY()
	TObjectPtr<UCommonButtonGroupBase> ButtonGroupPrivate;

	/** Generated buttons owned by this widget. */
	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> ButtonsPrivate;

	FNavigationDelegate OnNavigation;
};
