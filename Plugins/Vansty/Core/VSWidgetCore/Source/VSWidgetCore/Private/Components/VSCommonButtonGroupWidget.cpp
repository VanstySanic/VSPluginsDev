// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSCommonButtonGroupWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VSIndexImageGroupWidget.h"
#include "Groups/CommonButtonGroupBase.h"

UVSCommonButtonGroupWidget::UVSCommonButtonGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bSupportButtonFocus(false)
	, bNavigationAllowWrapping(true)
	, bDifferRefreshment(false)
{
	SetIsFocusable(true);
	
	ButtonGroupPrivate = CreateDefaultSubobject<UCommonButtonGroupBase>(TEXT("ButtonGroup"));
}

void UVSCommonButtonGroupWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR
	if (IsDesignTime() || !bDifferRefreshment)
	{
		RefreshButtons();
	}
#elif
	if (!bDifferRefreshment)
	{
		RefreshButtons();
	}
#endif

	ButtonGroupPrivate->OnSelectedButtonBaseChanged.AddUniqueDynamic(this, &UVSCommonButtonGroupWidget::OnButtonGroupSelectionChanged);
	ButtonGroupPrivate->OnButtonBaseClicked.AddUniqueDynamic(this, &UVSCommonButtonGroupWidget::UVSCommonButtonGroupWidget::OnButtonGroupButtonClicked);
}

FNavigationReply UVSCommonButtonGroupWidget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	FNavigationReply Reply = Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);

	switch (InNavigationEvent.GetNavigationType())
	{
	case EUINavigation::Left:
		ButtonGroupPrivate->SelectPreviousButton(bNavigationAllowWrapping);
		break;
		
	case EUINavigation::Right:
		ButtonGroupPrivate->SelectNextButton(bNavigationAllowWrapping);
		break;
		
	default:
		break;
	}

	return Reply;
}

void UVSCommonButtonGroupWidget::RefreshButtons()
{
	if (!Panel_Buttons) return;

	Panel_Buttons->ClearChildren();
	ButtonGroupPrivate->RemoveAll();

	const TArray<UWidget*>& PrevButtons = Panel_Buttons->GetAllChildren();
	const int32 PrevNum = PrevButtons.Num();

	for (int i = PrevNum; i < ButtonNum; ++i)
	{
		UCommonButtonBase* Button = WidgetTree->ConstructWidget<UCommonButtonBase>(ButtonClass);
		if (!Button) continue;
		
		Panel_Buttons->AddChild(Button);
		ButtonsPrivate.Add(Button);

		Button->SetIsFocusable(bSupportButtonFocus);
	}
	
	for (UCommonButtonBase* Button : ButtonsPrivate)
	{
		ButtonSlotSettings.ApplyToWidget(Button);
		ButtonStyleSettings.ApplyToButton(Button);
	}

	for (int i = 0; i < ButtonsPrivate.Num(); ++i)
	{
		if (ButtonActionSettings.IsValidIndex(i))
		{
			ButtonActionSettings[i].ApplyToButton(ButtonsPrivate[i]);
		}
		else
		{
			break;
		}
	}
	
	ButtonGroupPrivate->AddWidgets(ButtonsPrivate);

#if WITH_EDITORONLY_DATA
	if (IsDesignTime())
	{
		ButtonGroupPrivate->SelectButtonAtIndex(PreviewIndex);
	}
#endif

	if (bSupportButtonFocus && DesiredFocusButtonIndex >= 0 && ButtonsPrivate.IsValidIndex(DesiredFocusButtonIndex))
	{
		SetDesiredFocusWidget(ButtonsPrivate[DesiredFocusButtonIndex].GetFName());
	}

	if (IndexImageGroup)
	{
		IndexImageGroup->ImageNum = ButtonNum;
		IndexImageGroup->RefreshIndexImages();
		IndexImageGroup->SetSelectedIndex(ButtonGroupPrivate->GetSelectedButtonIndex());
	}
	
	OnRefreshed_Native.Broadcast(this);
	OnRefreshed.Broadcast(this);
}

void UVSCommonButtonGroupWidget::OnButtonGroupSelectionChanged(UCommonButtonBase* AssociatedButton, int32 ButtonIndex)
{
#if WITH_EDITORONLY_DATA
	PreviewIndex = ButtonIndex;
#endif

	/** Update button selection. */
	const int32 ActualButtonNum = ButtonsPrivate.Num();
	for (int i = 0; i < ActualButtonNum; i++)
	{
		ButtonsPrivate[i]->SetIsSelected(i == ButtonGroupPrivate->GetSelectedButtonIndex());
	}
	
	if (IndexImageGroup)
	{
		IndexImageGroup->SetSelectedIndex(ButtonIndex);
	}
}

void UVSCommonButtonGroupWidget::OnButtonGroupButtonClicked(UCommonButtonBase* AssociatedButton, int32 ButtonIndex)
{
	ButtonGroupPrivate->SelectButtonAtIndex(ButtonIndex);
}
