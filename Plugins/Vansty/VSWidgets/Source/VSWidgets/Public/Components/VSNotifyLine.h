// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSMathTypes.h"
#include "VSNotifyLine.generated.h"

struct FVSWidgetNotifyParams;
class UImage;
class URichTextBlock;


/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSNotifyLine : public UUserWidget
{
	GENERATED_UCLASS_BODY()

	static UVSNotifyLine* CreateAndDisplay(UPanelWidget* ParentPanelWidget, TSubclassOf<UVSNotifyLine> Class, const FVSWidgetNotifyParams& NotifyParams);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Widget")
	void OnAlphaUpdated(const float Alpha);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichTextBlock_NotifyTitle;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Notify;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<URichTextBlock> RichTextBlock_NotifyContent;

private:
	FVSAlphaDurationBlend AlphaDurationBlend;
};
