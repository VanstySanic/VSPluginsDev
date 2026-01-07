// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSCommonButtonGroupWidget.h"
#include "VSPrivablic.h"
#include "Blueprint/WidgetTree.h"
#include "CommonUI/Private/CommonButtonTypes.h"
#include "Components/VSIndexImageGroupWidget.h"
#include "Groups/CommonButtonGroupBase.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonBase, RootButton, TWeakObjectPtr<class UCommonButtonInternalBase>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonButtonInternalBase, MyCommonButton, TSharedPtr<class SCommonButton>);

UVSCommonButtonGroupWidget::UVSCommonButtonGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bSupportButtonFocus(false)
	, bNavigationAllowWrapping(true)
	, bVerticalFlowDirection(false)
	, bDifferRefreshment(false)
{
	SetIsFocusable(true);
	
	ButtonGroupPrivate = CreateDefaultSubobject<UCommonButtonGroupBase>(TEXT("ButtonGroup"));
}

void UVSCommonButtonGroupWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!bDifferRefreshment
#if WITH_EDITOR
		|| IsDesignTime()
#endif
		)
	{
		RefreshButtons();
	}
}

bool UVSCommonButtonGroupWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	
	if (Button_Prev)
	{
		Button_Prev->OnClicked.AddDynamic(this, &UVSCommonButtonGroupWidget::OnButtonPrevClicked);
	}

	if (Button_Next)
	{
		Button_Next->OnClicked.AddDynamic(this, &UVSCommonButtonGroupWidget::OnButtonNextClicked);
	}
	
	OnNavigation.BindUObject(this, &UVSCommonButtonGroupWidget::HandleNavigation);
	ButtonGroupPrivate->OnSelectedButtonBaseChanged.AddUniqueDynamic(this, &UVSCommonButtonGroupWidget::OnButtonGroupSelectionChanged);
	ButtonGroupPrivate->OnButtonBaseClicked.AddUniqueDynamic(this, &UVSCommonButtonGroupWidget::UVSCommonButtonGroupWidget::OnButtonGroupButtonClicked);
	
	return true;
}

FNavigationReply UVSCommonButtonGroupWidget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	FNavigationReply Reply = Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);

	switch (InNavigationEvent.GetNavigationType())
	{
	case EUINavigation::Left:
	case EUINavigation::Right:
		if (!bVerticalFlowDirection)
		{
			Reply = FNavigationReply::Custom(OnNavigation);
		}
		break;

	case EUINavigation::Up:
	case EUINavigation::Down:
		if (bVerticalFlowDirection)
		{
			Reply = FNavigationReply::Custom(OnNavigation);
		}
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

	for (int i = 0; i < ButtonNum; ++i)
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
		ButtonGroupPrivate->SelectButtonAtIndex(EditorPreviewIndex);
	}
#endif

	if (bSupportButtonFocus && DesiredFocusButtonIndex >= 0 && ButtonsPrivate.IsValidIndex(DesiredFocusButtonIndex))
	{
		SetDesiredFocusWidget(ButtonsPrivate[DesiredFocusButtonIndex].GetFName());
	}
	
	OnRefreshed_Native.Broadcast(this);
	OnRefreshed.Broadcast(this);
}

void UVSCommonButtonGroupWidget::OnButtonGroupSelectionChanged(UCommonButtonBase* AssociatedButton, int32 ButtonIndex)
{
#if WITH_EDITORONLY_DATA
	EditorPreviewIndex = ButtonIndex;
#endif

	/** Update button selection. */
	const int32 ActualButtonNum = ButtonsPrivate.Num();
	for (int i = 0; i < ActualButtonNum; i++)
	{
		ButtonsPrivate[i]->SetIsSelected(i == ButtonGroupPrivate->GetSelectedButtonIndex());
	}
}

void UVSCommonButtonGroupWidget::OnButtonGroupButtonClicked(UCommonButtonBase* AssociatedButton, int32 ButtonIndex)
{
	ButtonGroupPrivate->SelectButtonAtIndex(ButtonIndex);
}

void UVSCommonButtonGroupWidget::OnButtonPrevClicked()
{
	ButtonGroupPrivate->SelectPreviousButton(bNavigationAllowWrapping);
}

void UVSCommonButtonGroupWidget::OnButtonNextClicked()
{
	ButtonGroupPrivate->SelectNextButton(bNavigationAllowWrapping);
}

TSharedPtr<SWidget> UVSCommonButtonGroupWidget::HandleNavigation(EUINavigation UINavigation)
{
	switch (UINavigation)
	{
	case EUINavigation::Left:
		if (!bVerticalFlowDirection)
		{
			ButtonGroupPrivate->SelectPreviousButton(bNavigationAllowWrapping);
		}
		break;
		
	case EUINavigation::Right:
		if (!bVerticalFlowDirection)
		{
			ButtonGroupPrivate->SelectNextButton(bNavigationAllowWrapping);
		}
		break;

	case EUINavigation::Up:
		if (bVerticalFlowDirection)
		{
			ButtonGroupPrivate->SelectPreviousButton(bNavigationAllowWrapping);
		}
		break;
		
	case EUINavigation::Down:
		if (bVerticalFlowDirection)
		{
			ButtonGroupPrivate->SelectNextButton(bNavigationAllowWrapping);
		}
		break;
		
	default:
		break;
	}
	
	switch (UINavigation)
	{
	case EUINavigation::Left:
	case EUINavigation::Right:
	case EUINavigation::Up:
	case EUINavigation::Down:
		if (bSupportButtonFocus && ButtonGroupPrivate->GetSelectedButtonBase())
		{
			/** Don't know why the internal button is always not focusable even if set. Need to hack in that way. */
			if (UCommonButtonInternalBase* InternalButton = VS_PRIVABLIC_MEMBER(ButtonGroupPrivate->GetSelectedButtonBase(), UCommonButtonBase, RootButton).Get())
			{
				if (TSharedPtr<class SCommonButton> SlateButton = VS_PRIVABLIC_MEMBER(InternalButton, UCommonButtonInternalBase, MyCommonButton))
				{
					if (SlateButton.IsValid())
					{
						FSlateApplication::Get().SetUserFocus(GetOwningLocalPlayer()->GetPlatformUserId(), SlateButton);
					}
				}
			}
		}
		break;
		
	default:
		break;
	}
	
	return nullptr;
}
