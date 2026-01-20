// Copyright VanstySanic. All Rights Reserved.

#include "WidgetControllers/VSSettingItemWidgetController.h"  
#include "VSSettingSubsystem.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

UVSSettingItemWidgetController::UVSSettingItemWidgetController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

#if WITH_EDITOR
void UVSSettingItemWidgetController::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemWidgetController, ItemTag))
	{
		if (IsRegistered())
		{
			Reregister();
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemWidgetController::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
	{
		if (UVSSettingItem* SettingItem = SettingSubsystem->GetSettingItemByTag(ItemTag))
		{
			SettingItemPrivate = SettingItem;
			SettingItem->OnUpdated.AddDynamic(this, &UVSSettingItemWidgetController::OnSettingItemUpdated);
		}
	}
}

void UVSSettingItemWidgetController::Uninitialize_Implementation()
{
	if (SettingItemPrivate.IsValid())
	{
		SettingItemPrivate->OnUpdated.RemoveDynamic(this, &UVSSettingItemWidgetController::OnSettingItemUpdated);
	}
	SettingItemPrivate.Reset();

	Super::Uninitialize_Implementation();
}

void UVSSettingItemWidgetController::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (!SettingItemPrivate.Get()) return;

	if (TypeName == FName("Name"))
	{
		const FText& TextToSet = SettingItemPrivate->GetItemInfo().DisplayName.IsEmpty()
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

UVSSettingItem* UVSSettingItemWidgetController::GetSettingItem() const
{
	return SettingItemPrivate.Get();
}

void UVSSettingItemWidgetController::OnSettingItemUpdated_Implementation(UVSSettingItem* SettingItem)
{
	
}
