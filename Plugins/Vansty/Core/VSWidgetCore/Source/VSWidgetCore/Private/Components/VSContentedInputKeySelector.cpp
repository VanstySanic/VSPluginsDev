// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSContentedInputKeySelector.h"
#include "VSPrivablic.h"
#include "Blueprint/WidgetTree.h"
#include "Classes/VSKeyIconConfig.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/VSInputKeySelector.h"
#include "Widgets/Input/SInputKeySelector.h"

VS_DECLARE_PRIVABLIC_MEMBER(UTextBlock, MyTextBlock, TSharedPtr<STextBlock>);
VS_DECLARE_PRIVABLIC_MEMBER(SInputKeySelector, TextBlock, TSharedPtr<STextBlock>);

UVSContentedInputKeySelector::UVSContentedInputKeySelector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
	SetDesiredFocusWidget("InputKeySelector");
}

void UVSContentedInputKeySelector::NativePreConstruct()
{
	Super::NativePreConstruct();

	InputKeySelector->SetTextBlockVisibility(ESlateVisibility::Hidden);
	Panel_Content->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (InputKeySelector && Panel_Content)
	{
		InputKeySelector->OnKeySelected.AddUniqueDynamic(this, &UVSContentedInputKeySelector::OnKeySelected);
		InputKeySelector->OnIsSelectingKeyChanged.AddUniqueDynamic(this, &UVSContentedInputKeySelector::UVSContentedInputKeySelector::OnIsSelectingChanged);
	}

	RefreshContents();
}

void UVSContentedInputKeySelector::RefreshContents()
{
	if (!InputKeySelector || !Panel_Content) return;
	Panel_Content->ClearChildren();

	const FInputChord SelectedKey = InputKeySelector->GetSelectedKey();
	if (InputKeySelector->GetIsSelectingKey())
	{
		AddCustomText(InputKeySelector->GetKeySelectionText());
	}
	else if (!SelectedKey.Key.IsValid())
	{
		AddCustomText(InputKeySelector->GetNoKeySpecifiedText());
	}
	else
	{
		if (SelectedKey.HasAnyModifierKeys())
		{
			FKey ModifiedKey = FKey();
			if (SelectedKey.bAlt) ModifiedKey = EKeys::LeftAlt;
			else if (SelectedKey.bCmd) ModifiedKey =  EKeys::LeftAlt;
			else if (SelectedKey.bCtrl) ModifiedKey =  EKeys::LeftControl;
			else if (SelectedKey.bShift) ModifiedKey =  EKeys::LeftShift;

			if (!AddKeyIcon(ModifiedKey))
			{
				FString ModifiedString = SelectedKey.GetModifierText().ToString();
				ModifiedString.RemoveFromEnd("+");
				AddCustomText(FText::FromString(ModifiedString));
			}
			
			AddCustomText(FText::FromString(" + "));
		}

		if (!AddKeyIcon(SelectedKey.Key))
		{
			AddKeyText(SelectedKey.Key);
		}
	}
}

bool UVSContentedInputKeySelector::AddKeyIcon(const FKey& Key)
{
	if (!KeyIconConfig || !Panel_Content) return false;

	FVSCommonPanelSlotSettings DefaultSlotSettings;
	DefaultSlotSettings.bOverrideVerticalAlignment = true;
	DefaultSlotSettings.bOverrideHorizontalAlignment = true;
	DefaultSlotSettings.VerticalAlignment = VAlign_Center;
	DefaultSlotSettings.HorizontalAlignment = HAlign_Center;
	
	if (UTexture2D* Icon = KeyIconConfig->GetIconByKey(Key))
	{
		FSlateBrush Brush = KeyBrush;
		Brush.SetResourceObject(Icon);

		if (UImage* Image = WidgetTree->ConstructWidget<UImage>())
		{
			Panel_Content->AddChild(Image);
			Image->SetBrush(Brush);
			
			DefaultSlotSettings.ApplyToWidget(Image);
			ContentPanelSlotSettings.ApplyToWidget(Image);
			
			return true;
		}
	}

	return false;
}

void UVSContentedInputKeySelector::AddKeyText(const FKey& Key)
{
	if (!InputKeySelector || !Panel_Content) return;
	
	FVSCommonPanelSlotSettings DefaultSlotSettings;
	DefaultSlotSettings.bOverrideVerticalAlignment = true;
	DefaultSlotSettings.bOverrideHorizontalAlignment = true;
	DefaultSlotSettings.VerticalAlignment = VAlign_Center;
	DefaultSlotSettings.HorizontalAlignment = HAlign_Center;
	
	if (UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>())
	{
		const FTextBlockStyle TextStyle = InputKeySelector->GetTextStyle();
		
		TextBlock->SetFont(TextStyle.Font);
		TextBlock->SetColorAndOpacity(TextStyle.ColorAndOpacity);
		TextBlock->SetShadowColorAndOpacity(TextStyle.ShadowColorAndOpacity);
		TextBlock->SetShadowOffset(TextStyle.ShadowOffset);
		TextBlock->SetText(Key.GetDisplayName());
		TextBlock->SetStrikeBrush(TextStyle.StrikeBrush);

		Panel_Content->AddChild(TextBlock);
		
		DefaultSlotSettings.ApplyToWidget(TextBlock);
		ContentPanelSlotSettings.ApplyToWidget(TextBlock);
	}
}

void UVSContentedInputKeySelector::AddCustomText(const FText Text)
{
	if (!InputKeySelector || !Panel_Content) return;

	FVSCommonPanelSlotSettings DefaultSlotSettings;
	DefaultSlotSettings.bOverrideVerticalAlignment = true;
	DefaultSlotSettings.bOverrideHorizontalAlignment = true;
	DefaultSlotSettings.VerticalAlignment = VAlign_Center;
	DefaultSlotSettings.HorizontalAlignment = HAlign_Center;
	
	if (UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>())
	{
		const FTextBlockStyle TextStyle = InputKeySelector->GetTextStyle();
		TextBlock->SetFont(TextStyle.Font);
		TextBlock->SetColorAndOpacity(TextStyle.ColorAndOpacity);
		TextBlock->SetShadowColorAndOpacity(TextStyle.ShadowColorAndOpacity);
		TextBlock->SetShadowOffset(TextStyle.ShadowOffset);
		
		TextBlock->SetText(Text);

		Panel_Content->AddChild(TextBlock);
		DefaultSlotSettings.ApplyToWidget(TextBlock);
		ContentPanelSlotSettings.ApplyToWidget(TextBlock);
	}
}

void UVSContentedInputKeySelector::OnIsSelectingChanged()
{
	RefreshContents();
}

void UVSContentedInputKeySelector::OnKeySelected(FInputChord SelectedKey)
{
	RefreshContents();
}
