// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/VSSettingItemWidgetController.h"  
#include "VSSettingSubsystem.h"
#include "VSSettingSystemUtils.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Types/Math/VSContainer.h"
#include "Mediator/Binders/VSWidgetBinder.h"

UVSSettingItemWidgetController::UVSSettingItemWidgetController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

#if WITH_EDITOR
void UVSSettingItemWidgetController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemWidgetController, ItemIdentifier))
	{
		const bool PrevRegistered = IsRegistered();
		if (PrevRegistered)
		{
			Unregister();
		}

		SettingItemPrivate = UVSSettingSystemUtils::GetSettingItemByIdentifier(ItemIdentifier);
		if (!SettingItemPrivate.IsValid())
		{
			SettingItemPrivate = GetSettingItem_Native();
		}
		Execute_EditorRefreshMediator(this);
		SettingItemPrivate = nullptr;

		if (PrevRegistered)
		{
			Register();
		}
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemWidgetController, WidgetBinders))
	{
		if (LastEditorWidgetBinders != WidgetBinders)
		{
			const auto Diff = FVSArray::GetArrayDifference(WidgetBinders, LastEditorWidgetBinders);
			for (UVSWidgetBinder* WidgetBinder : Diff)
			{
				if (WidgetBinder)
				{
					Execute_EditorRefreshMediator(WidgetBinder);
				}
			}
		}
		
		LastEditorWidgetBinders = WidgetBinders;
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemWidgetController::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	LastEditorWidgetBinders = WidgetBinders;
#endif
}

void UVSSettingItemWidgetController::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
	{
		if (UVSSettingItemBase* SettingItem = UVSSettingSystemUtils::GetSettingItemByIdentifier(ItemIdentifier))
		{
			SettingItemPrivate = SettingItem;
			SettingItemPrivate->OnUpdated_Native.AddUObject(this, &UVSSettingItemWidgetController::OnCurrentSettingItemUpdatedNative);
		}
		SettingSubsystem->OnItemUpdated.AddDynamic(this, &UVSSettingItemWidgetController::OnAnySettingItemUpdated);
	}
}

void UVSSettingItemWidgetController::Uninitialize_Implementation()
{
	if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
	{
		SettingSubsystem->OnItemUpdated.RemoveDynamic(this, &UVSSettingItemWidgetController::OnAnySettingItemUpdated);
		if (UVSSettingItemBase* SettingItem = UVSSettingSystemUtils::GetSettingItemByIdentifier(ItemIdentifier))
		{
			SettingItem->OnUpdated_Native.RemoveAll(this);
		}
	}
	SettingItemPrivate.Reset();

	Super::Uninitialize_Implementation();
}

void UVSSettingItemWidgetController::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);
	
	if (TypeName == FName("Item"))
	{
		if (HasBeenInitialized())
		{
#if WITH_EDITOR
			if (!Widget->IsDesignTime())
#endif
			{
				Widget->SetVisibility(!SettingItemPrivate.IsValid() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
			}
		}
	}
	if (TypeName == FName("Name"))
	{
		if (!SettingItemPrivate.IsValid()) return; 
		const FText TextToSet = SettingItemPrivate->GetItemInfo().DisplayName.IsEmpty()
			? FText::FromString("Item Name")
			: SettingItemPrivate->GetItemInfo().DisplayName;
		
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(TextToSet);
		}
		else if (URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget))
		{
			RichTextBlock->SetText(TextToSet);
		}
	}
}

void UVSSettingItemWidgetController::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Name"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(FText::FromString("None"));
		}
		else if (URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget))
		{
			RichTextBlock->SetText(FText::FromString("None"));
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

void UVSSettingItemWidgetController::OnCurrentSettingItemUpdated_Implementation()
{
	for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
	{
		if (WidgetBinder && WidgetBinder->GetClass()->ImplementsInterface(UVSSettingItemWidgetMediatorInterface::StaticClass()))
		{
			Execute_OnCurrentSettingItemUpdated(WidgetBinder);
		}
	}
}

void UVSSettingItemWidgetController::OnAnySettingItemUpdated_Implementation(UVSSettingItemBase* SettingItem)
{
	for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
	{
		if (WidgetBinder && WidgetBinder->GetClass()->ImplementsInterface(UVSSettingItemWidgetMediatorInterface::StaticClass()))
		{
			Execute_OnAnySettingItemUpdated(WidgetBinder, SettingItem);
		}
	}
}

void UVSSettingItemWidgetController::EditorRefreshMediator_Implementation()
{
	for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
	{
		if (WidgetBinder && WidgetBinder->GetClass()->ImplementsInterface(UVSSettingItemWidgetMediatorInterface::StaticClass()))
		{
			Execute_EditorRefreshMediator(WidgetBinder);
		}
	}
}

UVSSettingItemBase* UVSSettingItemWidgetController::GetSettingItem_Implementation() const
{
	return SettingItemPrivate.Get();
}

void UVSSettingItemWidgetController::OnCurrentSettingItemUpdatedNative(UVSSettingItemBase* SettingItem)
{
	Execute_OnCurrentSettingItemUpdated(this);
}
