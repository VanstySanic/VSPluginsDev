// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetTypes.h"
#include "VSGroupWidget.generated.h"

class UCommonWidgetGroupBase;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSGroupWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	virtual void NativePreConstruct() override;

public:
	/** Get the widget group object that actually handles the group and index.. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UCommonWidgetGroupBase* GetWidgetGroup() const { return WidgetGroup; }

	/**
	 * Set the count of widgets in the group.
	 * All previous widgets in the group will be cleared, and new ones will be generated.
	 */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetWidgetNum(int32 InWidgetNum);

	/** Get the widgets in the group. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	TArray<UWidget*> GetWidgets() const;

private:
	void RefreshWidgets();
	void RefreshWidgetGroup();

protected:
	/** The panel that accommodates the group widget. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UCommonWidgetGroupBase> WidgetGroupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UWidget> WidgetClass;

	/** Defines the count of widgets in the group. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	int32 WidgetNum = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FVSPanelSlotLayoutParams WidgetSlotLayoutParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	ESlateVisibility WidgetVisibility = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bRefreshWidgetsByDefault = false;

private:
	UPROPERTY()
	TObjectPtr<UCommonWidgetGroupBase> WidgetGroup;
};
