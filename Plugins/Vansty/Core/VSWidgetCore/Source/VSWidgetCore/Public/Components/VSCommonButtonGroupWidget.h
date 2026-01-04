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
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSCommonButtonGroupWidget : public UCommonButtonBase
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRefreshedDelegate, UVSCommonButtonGroupWidget* /** Widget */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshedEvent, UVSCommonButtonGroupWidget*, Widget);

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	//~ End UUserWidget Interface

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
	UFUNCTION()
	void OnButtonGroupSelectionChanged(UCommonButtonBase* AssociatedButton, int32 ButtonIndex);
	
	UFUNCTION()
	void OnButtonGroupButtonClicked(UCommonButtonBase* AssociatedButton, int32 ButtonIndex);
	
public:
	FOnRefreshedDelegate OnRefreshed_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnRefreshedEvent OnRefreshed;
	
public:
	/** Button class to generate in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	TSubclassOf<UCommonButtonBase> ButtonClass;

	/** Button count in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	int32 ButtonNum = 1;
	
	/** Whether the buttons are focusable in the group (reactable by keyboard or gamepad). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bSupportButtonFocus : 1;

	/** Only works when greater than zero. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	int32 DesiredFocusButtonIndex = INDEX_NONE;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Group")
	uint8 bNavigationAllowWrapping : 1;
	
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
	/** The panel widget that contains the buttons. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Button Group")
	TObjectPtr<UPanelWidget> Panel_Buttons;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Button Group")
	TObjectPtr<UVSIndexImageGroupWidget> IndexImageGroup;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Button Group")
	int32 PreviewIndex = INDEX_NONE;
#endif

private:
	UPROPERTY()
	TObjectPtr<UCommonButtonGroupBase> ButtonGroupPrivate;

	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> ButtonsPrivate;
};
