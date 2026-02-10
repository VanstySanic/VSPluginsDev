// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/SlateWrapperTypes.h"
#include "Widgets/Layout/Anchors.h"
#include "VSWidgetSlotTypes.generated.h"

class UWidget;

USTRUCT(BlueprintType)
struct VSWIDGETCORE_API FVSCommonPanelSlotSettings
{
	GENERATED_USTRUCT_BODY()

	FVSCommonPanelSlotSettings(const UWidget* Widget = nullptr);
	void ApplyToWidget(UWidget* Widget);

	/** This may not work for all slots. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideSize"))
	FSlateChildSize Size = FSlateChildSize(ESlateSizeRule::Fill);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverridePadding"))
	FMargin Padding = FMargin();
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideHorizontalAlignment"))
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Fill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideVerticalAlignment"))
	TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Fill;
	
	/** This may not work for all slots. */
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideSize : 1;
	
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverridePadding : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideHorizontalAlignment : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideVerticalAlignment : 1;
};

/** Panel slot settings that works for a canvas panel widget. */
USTRUCT(BlueprintType)
struct VSWIDGETCORE_API FVSCanvasPanelSlotSettings
{
	GENERATED_BODY()

	FVSCanvasPanelSlotSettings(const UWidget* Widget = nullptr);
	void ApplyToWidget(UWidget* Widget);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideAnchors"))
	FAnchors Anchors = FAnchors(0.f, 0.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideOffsets"))
	FMargin Offsets = FMargin();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideAlignment"))
	FVector2D Alignment = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideAutoSize"))
	int32 ZOrder = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideZOrder"))
	uint8 bAutoSize : 1;
	
	
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideAnchors : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideOffsets : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideAlignment : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideAutoSize : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideZOrder : 1;
};