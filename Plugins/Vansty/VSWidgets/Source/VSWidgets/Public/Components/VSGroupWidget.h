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
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UCommonWidgetGroupBase* GetWidgetGroup() const { return WidgetGroup; }

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetWidgetNum(int32 InWidgetNum);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	TArray<UWidget*> GetWidgets() const;

private:
	void RefreshWidgets();
	void RefreshWidgetGroup();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UCommonWidgetGroupBase> WidgetGroupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UWidget> WidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	int32 WidgetNum = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FVSWidgetLayoutParams WidgetLayoutParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	ESlateVisibility WidgetVisibility = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bRefreshWidgetsByDefault = false;

private:
	UPROPERTY()
	TObjectPtr<UCommonWidgetGroupBase> WidgetGroup;
};
