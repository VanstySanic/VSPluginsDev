// Copyright VanstySanic. All Rights Reserved.

#include "Generic/VSMessageDialog.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "VSWidgetLibrary.h"
#include "Blueprint/WidgetTree.h"
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
		MessageDialog->CachedUIInputConfig = UVSWidgetLibrary::GetCurrentUIInputConfig(PlayerController);
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
		const int32 ZOrder = UVSWidgetLibrary::GetViewportMaxWidgetZOrder(PlayerController);
		MessageDialog->AddToPlayerScreen(ZOrder);
	}
	
	return MessageDialog;
}

bool UVSMessageDialog::Initialize()
{
	if (!Super::Initialize()) return false;
	if (!Panel_Button || !RichTextBlock_Message) return false;

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
		CachedUIInputConfig = UVSWidgetLibrary::GetCurrentUIInputConfig(GetOwningPlayer());
	}

	Super::NativeOnActivated();
}

void UVSMessageDialog::NativeOnDeactivated()
{
	if (bRestoreActiveUIInputConfig)
	{
		UVSWidgetLibrary::SetCurrentUIInputConfig(GetOwningPlayer(), CachedUIInputConfig);
	}
	
	Super::NativeOnDeactivated();
}

void UVSMessageDialog::GenerateAndInitializeButtons()
{
	/** Clear original buttons in the panel. */
	for (UWidget* Child : Panel_Button->GetAllChildren())
	{
		if (UCommonButtonBase* Button = Cast<UCommonButtonBase>(Child))
		{
			Button->RemoveFromParent();
		}
		else
		{
			Panel_Button->RemoveChild(Child);
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
			ButtonLayoutParams.ApplyToWidget(Button);
			
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
}

void UVSMessageDialog::OnDialogReplied()
{
	if (bRestoreActiveUIInputConfig)
	{
		UVSWidgetLibrary::SetCurrentUIInputConfig(GetOwningPlayer(), CachedUIInputConfig);
	}
	
	if (IsActivated())
	{
		DeactivateWidget();
	}

	/** TODO Change to another way. */
	RemoveFromParent();
}
