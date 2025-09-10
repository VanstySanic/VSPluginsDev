// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSComboBoxString.h"

UVSComboBoxString::UVSComboBoxString(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSComboBoxString::SetTextJustification(ETextJustify::Type InJustification)
{
	TextJustification = InJustification;
	if (!OnGenerateWidgetEvent.IsBound() && DefaultComboBoxContent.IsValid())
	{
		DefaultComboBoxContent.Pin()->SetJustification(TextJustification);
	}
}

TSharedRef<SWidget> UVSComboBoxString::HandleGenerateWidget(TSharedPtr<FString> Item) const
{
	const FString StringItem = Item.IsValid() ? *Item : FString();

	if (OnGenerateWidgetEvent.IsBound())
	{
		UWidget* Widget = OnGenerateWidgetEvent.Execute(StringItem);
		if (Widget != nullptr)
		{
			return Widget->TakeWidget();
		}
	}

	/** If a row wasn't generated just create the default one, a simple text block of the item's name. */
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		return SNew(STextBlock)
			.Text(FText::FromString(StringItem))
			.Font(Font)
			.Justification(TextJustification);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}
