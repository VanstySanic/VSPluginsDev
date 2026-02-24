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
 * A CommonUI-based widget that manages a group of buttons as a single
 * selectable option set.
 *
 * This widget dynamically generates and owns a collection of CommonButton
 * instances, routes selection and navigation through an internal
 * UCommonButtonGroupBase, and exposes unified focus, navigation, and
 * refresh behavior to both C++ and Blueprint.
 *
 * It supports controller/keyboard navigation (horizontal or vertical),
 * optional wrapping, external Prev/Next controls, and per-index button
 * configuration through style and action settings.
 *
 * The button group can be refreshed at runtime to reflect changes in
 * option count, layout, or binding state, and notifies listeners when
 * regeneration is complete.
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
	/**
	 * Regenerate all buttons in response to changes in the count or layout.
	 * @note Don't forget to bind your delegates for buttons after the refreshment if necessary.
	 */
	UFUNCTION(BlueprintCallable, Category = "Button Group")
	void RefreshButtons();
	
	/**
	 * Get the button group object to bind delegates or process selection.
	 * @warning Do not add, remove or modify buttons here.
	 */
	UFUNCTION(BlueprintCallable, Category = "Button Group")
	UCommonButtonGroupBase* GetButtonGroup() const { return ButtonGroupPrivate; }

private:
	/** Handle and use controller navigation to rotate text */
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
	FOnRefreshedDelegate OnRefreshed_Native;
	FOnSelectionChangedDelegate OnSelectionChanged_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnRefreshedEvent OnRefreshed;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnSelectionChangedEvent OnSelectionChanged;
	
public:
	/** Button class to generate in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	TSubclassOf<UCommonButtonBase> ButtonClass;

	/** Button count in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	int32 ButtonNum = 1;

	/** Override the button action names if not empty, and the name in array is not empty. */
	UPROPERTY(EditAnywhere, Category = "Rotator")
	TArray<FText> OverridenButtonNames;

	/** Disable buttons of these indexes when refreshed. */
	UPROPERTY(EditAnywhere, Category = "Rotator")
	TArray<int32> DefaultDisabledIndexes;
	
	/** Only works when greater than zero. */
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
	 * If false, a refreshment of buttons will be done during pre-construction.
	 * If true, button refreshment should be executed manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bDifferRefreshment : 1;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	FVSCommonPanelSlotSettings ButtonSlotSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	FVSCommonButtonStyleSettings ButtonStyleSettings;

	/** Apply by index. */
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

	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> ButtonsPrivate;

	FNavigationDelegate OnNavigation;
};
