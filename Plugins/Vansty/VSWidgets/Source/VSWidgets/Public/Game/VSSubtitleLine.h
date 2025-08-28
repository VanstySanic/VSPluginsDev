// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSWidgets/Public/Types/VSWidgetTypes.h"
#include "VSSubtitleLine.generated.h"

struct FVSWidgetSubtitleParams;
class URichTextBlock;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSSubtitleLine : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	static UVSSubtitleLine* CreateAndDisplay(UPanelWidget* ParentPanelWidget, TSubclassOf<UVSSubtitleLine> Class, const FVSWidgetSubtitleParams& SubtitleParams);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Widget")
	void OnAlphaUpdated(const float Alpha);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichTextBlock_Subtitle;

		
private:
	FVSAlphaDurationBlend AlphaDurationBlend;
};
