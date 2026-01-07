// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSWidgetSlotTypes.h"
#include "Components/BorderSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"

FVSCommonPanelSlotSettings::FVSCommonPanelSlotSettings(const UWidget* Widget)
	: bOverrideSize(false)
	, bOverridePadding(false)
	, bOverrideHorizontalAlignment(false)
	, bOverrideVerticalAlignment(false)
{
	if (!Widget) return;

	if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
	{
		Size = HorizontalBoxSlot->GetSize();
		Padding = HorizontalBoxSlot->GetPadding();
		HorizontalAlignment = HorizontalBoxSlot->GetHorizontalAlignment();
		VerticalAlignment = HorizontalBoxSlot->GetVerticalAlignment();
	}
	else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
	{
		Size = VerticalBoxSlot->GetSize();
		Padding = VerticalBoxSlot->GetPadding();
		HorizontalAlignment = VerticalBoxSlot->GetHorizontalAlignment();
		VerticalAlignment = VerticalBoxSlot->GetVerticalAlignment();
	}
	else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Widget->Slot))
	{
		Padding = BorderSlot->GetPadding();
		HorizontalAlignment = BorderSlot->GetHorizontalAlignment();
		VerticalAlignment = BorderSlot->GetVerticalAlignment();
	}
	else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
	{
		Padding = ButtonSlot->GetPadding();
		HorizontalAlignment = ButtonSlot->GetHorizontalAlignment();
		VerticalAlignment = ButtonSlot->GetVerticalAlignment();
	}
	else if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
	{
		Padding = OverlaySlot->GetPadding();
		HorizontalAlignment = OverlaySlot->GetHorizontalAlignment();
		VerticalAlignment = OverlaySlot->GetVerticalAlignment();
	}
	else if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(Widget->Slot))
	{
		Size = ScrollBoxSlot->GetSize();
		Padding = ScrollBoxSlot->GetPadding();
		HorizontalAlignment = ScrollBoxSlot->GetHorizontalAlignment();
		VerticalAlignment = ScrollBoxSlot->GetVerticalAlignment();
	}
	else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
	{
		Padding = SizeBoxSlot->GetPadding();
		HorizontalAlignment = SizeBoxSlot->GetHorizontalAlignment();
		VerticalAlignment = SizeBoxSlot->GetVerticalAlignment();
	}
	else if (UGridSlot* GridSlot = Cast<UGridSlot>(Widget->Slot))
	{
		Padding = GridSlot->GetPadding();
		HorizontalAlignment = GridSlot->GetHorizontalAlignment();
		VerticalAlignment = GridSlot->GetVerticalAlignment();
	}
}

void FVSCommonPanelSlotSettings::ApplyToWidget(UWidget* Widget)
{
	if (!Widget) return;

	if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
	{
		if (bOverrideSize) HorizontalBoxSlot->SetSize(Size);
		if (bOverridePadding) HorizontalBoxSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) HorizontalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) HorizontalBoxSlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
	{
		if (bOverrideSize) VerticalBoxSlot->SetSize(Size);
		if (bOverridePadding) VerticalBoxSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) VerticalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) VerticalBoxSlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Widget->Slot))
	{
		if (bOverridePadding) BorderSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) BorderSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) BorderSlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Widget->Slot))
	{
		if (bOverridePadding) ButtonSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) ButtonSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) ButtonSlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
	{
		if (bOverridePadding) OverlaySlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) OverlaySlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) OverlaySlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(Widget->Slot))
	{
		if (bOverrideSize) ScrollBoxSlot->SetSize(Size);
		if (bOverridePadding) ScrollBoxSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) ScrollBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) ScrollBoxSlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(Widget->Slot))
	{
		if (bOverridePadding) SizeBoxSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) SizeBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) SizeBoxSlot->SetVerticalAlignment(VerticalAlignment);
	}
	else if (UGridSlot* GridSlot = Cast<UGridSlot>(Widget->Slot))
	{
		if (bOverridePadding) GridSlot->SetPadding(Padding);
		if (bOverrideHorizontalAlignment) GridSlot->SetHorizontalAlignment(HorizontalAlignment);
		if (bOverrideVerticalAlignment) GridSlot->SetVerticalAlignment(VerticalAlignment);
	}
}


FVSCanvasPanelSlotSettings::FVSCanvasPanelSlotSettings(const UWidget* Widget)
	: bAutoSize(false)
	, bOverrideAnchors(false)
	, bOverrideOffsets(false)
	, bOverrideAlignment(false)
	, bOverrideAutoSize(false)
	, bOverrideZOrder(false)
{
	if (!Widget) return;

	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{
		Anchors = CanvasPanelSlot->GetAnchors();
		Offsets = CanvasPanelSlot->GetOffsets();
		Alignment = CanvasPanelSlot->GetAlignment();
		bAutoSize = CanvasPanelSlot->GetAutoSize();
		ZOrder = CanvasPanelSlot->GetZOrder();
	}
}

void FVSCanvasPanelSlotSettings::ApplyToWidget(UWidget* Widget)
{
	if (!Widget) return;

	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{
		if (bOverrideAnchors) CanvasPanelSlot->SetAnchors(Anchors);
		if (bOverrideOffsets) CanvasPanelSlot->SetOffsets(Offsets);
		if (bOverrideAlignment) CanvasPanelSlot->SetAlignment(Alignment);
		if (bOverrideAutoSize) CanvasPanelSlot->SetAutoSize(bAutoSize);
		if (bOverrideZOrder) CanvasPanelSlot->SetZOrder(ZOrder);
	}
}
