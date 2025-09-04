// Copyright VanstySanic. All Rights Reserved.

#include "Generic/VSMessageDialog.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "VSWidgetUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSWidgetTypes.h"

UVSMessageDialog::UVSMessageDialog(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UVSMessageDialog* UVSMessageDialog::CreateMessageDialog(APlayerController* PlayerController, const TSubclassOf<UVSMessageDialog> Class, const FVSWidgetMessageDialogParams& Params, bool bAutoDisplay)
{
	if (!PlayerController || !Class || Params.DialogReplies.IsEmpty()) return nullptr;
	
	UVSMessageDialog* MessageDialog = CreateWidget<UVSMessageDialog>(PlayerController, Class);

	MessageDialog->MessageDialogParams = Params;
	MessageDialog->GenerateAndInitializeButtons();

	if (MessageDialog->bRestoreActiveUIInputConfig && MessageDialog->bAutoActivate)
	{
		MessageDialog->CachedUIInputConfig = UVSWidgetUtils::GetCurrentUIInputConfig(PlayerController);
	}
	
	if (MessageDialog->TextBlock_Title)
	{
		MessageDialog->TextBlock_Title->SetText(Params.DialogTitleText);
	}
	if (MessageDialog->RichTextBlock_Message)
	{
		MessageDialog->RichTextBlock_Message->SetText(Params.MessageContentText);
	}

	if (bAutoDisplay)
	{
		const int32 ZOrder = UVSGameplayLibrary::GetViewportMaxWidgetZOrder(PlayerController);
		MessageDialog->AddToPlayerScreen(ZOrder);
	}
	
	return MessageDialog;
}

bool UVSMessageDialog::Initialize()
{
	if (!Super::Initialize()) return false;
	if (!Panel_Button || !RichTextBlock_Message) return false;
	if (Button_Sample && !Panel_Button->HasChild(Panel_Button)) return false;

	return true;
}

void UVSMessageDialog::NativeDestruct()
{
	if (IsActivated())
	{
		DeactivateWidget();
	}
	
	Super::NativeDestruct();
}

void UVSMessageDialog::NativeOnActivated()
{
	/** Cache the ui input config befor set to new. */
	if (bRestoreActiveUIInputConfig)
	{
		CachedUIInputConfig = UVSWidgetUtils::GetCurrentUIInputConfig(GetOwningPlayer());
	}

	Super::NativeOnActivated();
}

void UVSMessageDialog::NativeOnDeactivated()
{
	if (bRestoreActiveUIInputConfig)
	{
		UVSWidgetUtils::SetCurrentUIInputConfig(GetOwningPlayer(), CachedUIInputConfig);
	}
	
	Super::NativeOnDeactivated();
}

void UVSMessageDialog::GenerateAndInitializeButtons()
{
	if (Button_Sample)
	{
		check(Panel_Button->HasChild(Button_Sample));
	}
	
	/** Clear original buttons in the panel. */
	for (UWidget* Child : Panel_Button->GetAllChildren())
	{
		if (UCommonButtonBase* Button = Cast<UCommonButtonBase>(Child))
		{
			if (Button_Sample && Button != Button_Sample)
			{
				Button->RemoveFromParent();
			}
		}	
	}

	/** Process reply for buttons. */
	for (FName Reply : MessageDialogParams.DialogReplies)
	{
		if (RepliedButtonClasses.Contains(Reply))
		{
			UCommonButtonBase* Button = CreateWidget<UCommonButtonBase>(this, RepliedButtonClasses.FindRef(Reply));
			NamedReplyGeneratedButtons.Emplace(Button, Reply);

			/** Refresh button display params. */
			if (RepliedButtonDisplayParams.Contains(Reply))
			{
				if (FVSCommonButtonDisplayParams* ButtonDisplayParams = RepliedButtonDisplayParams.FindRef(Reply).GetRow<FVSCommonButtonDisplayParams>(nullptr))
				{
					ButtonDisplayParams->ApplyToButton(Button);
				}
			}
			
			/** Setup panel. */
			Panel_Button->AddChild(Button);
			if (UHorizontalBoxSlot* PanelSlot = Cast<UHorizontalBoxSlot>(Button->Slot))
			{
				if (Button_Sample)
				{
					UHorizontalBoxSlot* SampleSlot = Cast<UHorizontalBoxSlot>(Button_Sample->Slot);
					PanelSlot->SetSize(SampleSlot->GetSize());
					PanelSlot->SetPadding(SampleSlot->GetPadding());
					PanelSlot->SetHorizontalAlignment(SampleSlot->GetHorizontalAlignment());
					PanelSlot->SetVerticalAlignment(SampleSlot->GetVerticalAlignment());
				}
				else
				{
					PanelSlot->SetSize(ESlateSizeRule::Fill);
					PanelSlot->SetHorizontalAlignment(HAlign_Center);
					PanelSlot->SetVerticalAlignment(VAlign_Fill);
				}
			}
			/** else if (false) {  } */
			
			if (MessageDialogParams.DefaultFocusReply == Reply)
			{
				/** Activation / focus will be set during next tick, so it's safe to set during construction. */
				SetDesiredFocusWidget(Button);
			}

			/** Bind button reply call-back delegate. */
			Button->OnClicked().AddLambda([&, Reply] ()
			{
				OnDialogReplied();
				OnReplied.Broadcast(Reply);
			});
		}
	}
	
	if (Button_Sample)
	{
		Button_Sample->RemoveFromParent();
	}
}

void UVSMessageDialog::OnDialogReplied()
{
	if (bRestoreActiveUIInputConfig)
	{
		UVSWidgetUtils::SetCurrentUIInputConfig(GetOwningPlayer(), CachedUIInputConfig);
	}
	
	if (IsActivated())
	{
		DeactivateWidget();
	}

	/** TODO Change to another way. */
	RemoveFromParent();
}
