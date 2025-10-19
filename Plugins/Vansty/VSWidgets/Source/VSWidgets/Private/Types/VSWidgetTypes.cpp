// Copyright VanstySanic. All Rights Reserved.

#include "VSWidgets/Public/Types/VSWidgetTypes.h"
#include "CommonActionWidget.h"
#include "CommonButtonBase.h"
#include "VSPrivablic.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, InputActionWidget, TObjectPtr<UCommonActionWidget>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, MinWidth, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, MinHeight, int32);

void FVSPanelSlotLayoutParams::ApplyToWidget(UWidget* Widget)
{
	if (!Widget) return;

	if (UHorizontalBoxSlot* PanelSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
	{
		PanelSlot->SetSize(Size);
		PanelSlot->SetPadding(Padding);
		PanelSlot->SetHorizontalAlignment(HorizontalAlignment);
		PanelSlot->SetVerticalAlignment(VerticalAlignment);
	}
}

void FVSCanvasSlotLayoutParams::ApplyToWidget(UWidget* Widget)
{
	if (!Widget) return;

	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{
		CanvasPanelSlot->SetAnchors(Anchors);
		CanvasPanelSlot->SetOffsets(Offsets);
		CanvasPanelSlot->SetAlignment(Alignment);
		CanvasPanelSlot->SetAutoSize(bAutoSize);
		CanvasPanelSlot->SetZOrder(ZOrder);
	}
}

FString FVSWidgetSubtitleParams::ToString() const
{
	FString TextToDisplay = !SpeakerName.IsEmpty() ?
		(!SpeakerNameTextStyle.ToString().IsEmpty() ? ("<" + SpeakerNameTextStyle.ToString() + ">" + SpeakerName.ToString() + "</>: ") : SpeakerName.ToString())
		: FString();

	TextToDisplay += SubtitleText.GetString();
	return TextToDisplay;
}

FText FVSWidgetSubtitleParams::ToText() const
{
	return FText::FromString(ToString());
}

void FVSCommonButtonDisplayParams::ApplyToButton(UCommonButtonBase* Button) const
{
	if (!Button) return;

	if (!DisplayName.IsEmpty() && !FName(DisplayName.ToString()).IsNone())
	{
		TArray<UWidget*> ChildWidgets;
		Button->WidgetTree->GetAllWidgets(ChildWidgets);
		for (UWidget* ChildWidget : ChildWidgets)
		{
			bool bDisplayNameSet = false;
			if (UTextBlock* TextBlock = Cast<UTextBlock>(ChildWidget))
			{
				TextBlock->SetText(DisplayName);
				bDisplayNameSet = true;
			}
			else if (URichTextBlock* RichTextBlock = Cast<URichTextBlock>(ChildWidget))
			{
				RichTextBlock->SetText(DisplayName);
				bDisplayNameSet = true;
			}

			if (bDisplayNameSet)
			{
				break;
			}
		}
	}

	if (MinimumSize.X >= 0.f)
	{
		Button->SetMinDimensions(MinimumSize.X, VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, MinHeight));
	}
	if (MinimumSize.Y >= 0.f)
	{
		Button->SetMinDimensions(VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, MinWidth), MinimumSize.Y);
	}
	
	if (Style)
	{
		Button->SetStyle();
	}

	if (UCommonActionWidget* ActionWidget = VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, InputActionWidget))
	{
		if (!InputActions.IsEmpty())
		{
			ActionWidget->SetInputActions(InputActions);
		}
		if (EnhancedInputAction)
		{
			ActionWidget->SetEnhancedInputAction(EnhancedInputAction);
		}
	}
}
