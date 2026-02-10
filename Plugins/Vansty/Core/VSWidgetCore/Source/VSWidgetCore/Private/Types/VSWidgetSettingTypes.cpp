// Copyright VanstySanic. All Rights Reserved.


#include "Types/VSWidgetSettingTypes.h"
#include "CommonActionWidget.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "VSPrivablic.h"
#include "Blueprint/WidgetTree.h"
#include "Input/CommonBoundActionButton.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCommonActionWidget, InputActions, TArray<FDataTableRowHandle>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonActionWidget, EnhancedInputAction, TObjectPtr<UInputAction>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, InputActionWidget, TObjectPtr<UCommonActionWidget>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, Style, TSubclassOf<UCommonButtonStyle>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, MinWidth, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, MinHeight, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, bHideInputAction, bool);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonBoundActionButton, Text_ActionName, TObjectPtr<UCommonTextBlock>);

VS_DECLARE_PRIVABLIC_METHOD(UUserWidget, SetInputActionPriority, void, int32);

FVSCommonActionWidgetSettings::FVSCommonActionWidgetSettings(const UCommonActionWidget* ActionWidget)
	: bOverrideInputActions(false)
	, bOverrideEnhancedInputAction(false)
	, bOverrideProgressMaterialBrush(false)
	, bOverrideProgressMaterialParam(false)
	, bOverrideIconRimBrush(false)
{
	if (!ActionWidget) return;

	InputActions = VS_PRIVABLIC_MEMBER(ActionWidget, UCommonActionWidget, InputActions);
	EnhancedInputAction = VS_PRIVABLIC_MEMBER(ActionWidget, UCommonActionWidget, EnhancedInputAction);
	ProgressMaterialBrush = ActionWidget->ProgressMaterialBrush;
	ProgressMaterialParam = ActionWidget->ProgressMaterialParam;
	IconRimBrush = ActionWidget->IconRimBrush;
}

void FVSCommonActionWidgetSettings::ApplyToAction(UCommonActionWidget* ActionWidget) const
{
	if (!ActionWidget) return;

	if (bOverrideInputActions) ActionWidget->SetInputActions(InputActions);
	if (bOverrideEnhancedInputAction) ActionWidget->SetEnhancedInputAction(EnhancedInputAction);
	if (bOverrideIconRimBrush) ActionWidget->SetIconRimBrush(IconRimBrush);

	if (bOverrideProgressMaterialBrush && bOverrideProgressMaterialParam) ActionWidget->SetProgressMaterial(ProgressMaterialBrush, ProgressMaterialParam);
	else if (bOverrideProgressMaterialBrush) ActionWidget->SetProgressMaterial(ProgressMaterialBrush, ActionWidget->ProgressMaterialParam);
	else if (bOverrideProgressMaterialParam) ActionWidget->SetProgressMaterial(ActionWidget->ProgressMaterialBrush, ProgressMaterialParam);
}


FVSCommonButtonStyleSettings::FVSCommonButtonStyleSettings(const UCommonButtonBase* Button)
	: bOverrideStyle(false)
	, bOverrideMinWidth(false)
	, bOverrideMinHeight(false)
{
	if (!Button) return;

	Style = VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, Style);
	MinWidth = VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, MinWidth);
	MinHeight = VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, MinHeight);
}

void FVSCommonButtonStyleSettings::ApplyToButton(UCommonButtonBase* Button) const
{
	if (!Button) return;

	if (bOverrideStyle) Button->SetStyle(Style);

	if (bOverrideMinWidth && bOverrideMinHeight) Button->SetMinDimensions(MinWidth, MinHeight);
	else if (bOverrideMinWidth) Button->SetMinDimensions(MinWidth, VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, MinHeight));
	else if (bOverrideMinHeight) Button->SetMinDimensions(VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, MinWidth), MinHeight);
}


FVSCommonButtonActionSettings::FVSCommonButtonActionSettings(const UCommonButtonBase* Button)
	: bOverrideInputAction(false)
	, bOverrideEnhancedInputAction(false)
	, bOverridePriority(false)
	, bOverrideActionName(false)
{
	ActionDisplaySettings.bOverrideInputActions = false;
	ActionDisplaySettings.bOverrideEnhancedInputAction = false;
	ActionDisplaySettings.bOverrideProgressMaterialBrush = false;
	ActionDisplaySettings.bOverrideProgressMaterialParam = false;
	ActionDisplaySettings.bOverrideIconRimBrush = false;
	
	if (!Button) return;

	Button->GetInputAction(InputAction);
	EnhancedInputAction = Button->GetEnhancedInputAction();
	Priority = Button->GetInputActionPriority();
	
	ActionDisplaySettings = FVSCommonActionWidgetSettings(VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, InputActionWidget));
	if (const UCommonBoundActionButton* ActionButton = Cast<UCommonBoundActionButton>(Button))
	{
		if (UCommonTextBlock* TextBlock = VS_PRIVABLIC_MEMBER(ActionButton, UCommonBoundActionButton, Text_ActionName))
		{
			ActionName = TextBlock->GetText();
		}
	}
	else
	{
		TArray<UWidget*> Widgets;
		Button->WidgetTree->GetAllWidgets(Widgets);
		for (UWidget* Widget : Widgets)
		{
			if (Widget && Widget->GetFName() == FName("Text_ActionName"))
			{
				if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
				{
					ActionName = TextBlock->GetText();
				}
			}
		}
	}
}

void FVSCommonButtonActionSettings::ApplyToButton(UCommonButtonBase* Button) const
{
	if (!Button) return;

	if (bOverrideInputAction) Button->SetTriggeringInputAction(InputAction);
	if (bOverrideEnhancedInputAction) Button->SetTriggeringEnhancedInputAction(EnhancedInputAction);
	if (bOverridePriority) VS_PRIVABLIC_METHOD(Button, UUserWidget,  SetInputActionPriority)(Priority);

	ActionDisplaySettings.ApplyToAction(VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, InputActionWidget));
	
	if (bOverrideActionName)
	{
		auto ActionWidget = VS_PRIVABLIC_MEMBER(Button, UCommonButtonBase, InputActionWidget);
		if (!ActionWidget)
		{
			if (const UCommonBoundActionButton* ActionButton = Cast<UCommonBoundActionButton>(Button))
			{
				if (UCommonTextBlock* TextBlock = VS_PRIVABLIC_MEMBER(ActionButton, UCommonBoundActionButton, Text_ActionName))
				{
					TextBlock->SetText(ActionName);
				}
			}
			else
			{
				TArray<UWidget*> Widgets;
				Button->WidgetTree->GetAllWidgets(Widgets);
				for (UWidget* Widget : Widgets)
				{
					if (Widget && Widget->GetFName() == FName("Text_ActionName"))
					{
						if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
						{
							TextBlock->SetText(ActionName);
						}
					}
				}
			}
		}
	}
}
