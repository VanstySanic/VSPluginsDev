// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/VSHighlightableWidget.h"
#include "Components/TextBlock.h"
#include "VSSettingItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemWidget : public UVSHighlightableWidget
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UTextBlock> TextBlock_Name;
};
