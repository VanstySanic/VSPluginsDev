// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSOptionBasedWidgetBinder.h"
#include "CommonRotator.h"
#include "VSWidgetController.h"
#include "Components/ComboBoxString.h"
#include "Components/VSCommonButtonGroupWidget.h"
#include "Groups/CommonButtonGroupBase.h"

UVSOptionBasedWidgetBinder::UVSOptionBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSOptionBasedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	RefreshOptions();

	FInternationalization::Get().OnCultureChanged().AddUObject(this, &UVSOptionBasedWidgetBinder::OnCultureChanged);
}

void UVSOptionBasedWidgetBinder::Uninitialize_Implementation()
{
	FInternationalization::Get().OnCultureChanged().RemoveAll(this);

	Super::Uninitialize_Implementation();
}

void UVSOptionBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	const TMap<FString, FText>& KeyedTexts = GetOptionKeyedTexts();
	TArray<FString> OptionKeys;
	TArray<FText> OptionTexts;
	KeyedTexts.GetKeys(OptionKeys);
	KeyedTexts.GenerateValueArray(OptionTexts);

	int32 SelectedIndex = GetSelectedIndex();
	
	if (TypeName == FName("Value"))
	{
		if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(Widget))
		{
			ComboBoxString->ClearOptions();
			
			for (const FText& OptionText : OptionTexts)
			{
				ComboBoxString->AddOption(OptionText.ToString());
			}
			
			ComboBoxString->RefreshOptions();
			ComboBoxString->SetSelectedIndex(SelectedIndex);
		}
		else if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(Widget))
		{
			CommonRotator->ClearSelection();
			CommonRotator->PopulateTextLabels(OptionTexts);
			CommonRotator->SetSelectedItem(SelectedIndex);
		}
		else if (UVSCommonButtonGroupWidget* ButtonGroupWidget = Cast<UVSCommonButtonGroupWidget>(Widget))
		{
			ButtonGroupWidget->ButtonActionSettings.Reserve(OptionTexts.Num());
			
			for (int i = 0; i < OptionTexts.Num(); ++i)
			{
				if (ButtonGroupWidget->ButtonActionSettings.Num() < i)
				{
					ButtonGroupWidget->ButtonActionSettings[i].ActionName = OptionTexts[i];
				}
				else
				{
					FVSCommonButtonActionSettings CommonButtonActionSettings;
					CommonButtonActionSettings.bOverrideActionName = true;
					CommonButtonActionSettings.ActionName = OptionTexts[i];
					ButtonGroupWidget->ButtonActionSettings.Add(CommonButtonActionSettings);
				}
			}

			ButtonGroupWidget->RefreshButtons();
			ButtonGroupWidget->GetButtonGroup()->SelectButtonAtIndex(SelectedIndex);
		}
	}
}

void UVSOptionBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Value"))
	{
		if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(Widget))
		{
			ComboBoxString->ClearOptions();
		}
		else if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(Widget))
		{
			CommonRotator->ClearSelection();
		}
		else if (UVSCommonButtonGroupWidget* ButtonGroupWidget = Cast<UVSCommonButtonGroupWidget>(Widget))
		{
			
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

FString UVSOptionBasedWidgetBinder::GetCurrentOptionKey_Implementation() const
{
	return FString();
}

void UVSOptionBasedWidgetBinder::RefreshOptions()
{
	OptionKeyedTexts = GetOptionKeyedTexts();
	RebindWidgetByType(FName("Value"));
}

TMap<FString, FText> UVSOptionBasedWidgetBinder::GetOptionKeyedTexts_Implementation() const
{
	return TMap<FString, FText>();
}

int32 UVSOptionBasedWidgetBinder::GetSelectedIndex() const
{
	int32 SelectedIndex = INDEX_NONE;
	const FString& CurrentOptionKey = GetCurrentOptionKey();
	for (const TPair<FString, FText>& CachedOptionKeyedText : OptionKeyedTexts)
	{
		SelectedIndex++;
		if (CachedOptionKeyedText.Key == CurrentOptionKey)
		{
			return SelectedIndex;
		}
	}

	return INDEX_NONE;
}

FText UVSOptionBasedWidgetBinder::GetCurrentOptionText() const
{
	return OptionKeyedTexts.FindRef(GetCurrentOptionKey());
}

FText UVSOptionBasedWidgetBinder::GetOptionTextByKey(const FString& String) const
{
	return OptionKeyedTexts.FindRef(String);
}

FString UVSOptionBasedWidgetBinder::GetOptionKeyByText(const FText& Text) const
{
	static FString EmptyString = FString();
	
	for (const TPair<FString, FText>& CachedOptionKeyedText : OptionKeyedTexts)
	{
		if (CachedOptionKeyedText.Value.EqualTo(Text))
		{
			return CachedOptionKeyedText.Key;
		}
	}

	return EmptyString;
}

void UVSOptionBasedWidgetBinder::OnWidgetOptionChanged_Implementation(const FString& NewKey, const FText& NewText, int32 NewIndex)
{
	
}

void UVSOptionBasedWidgetBinder::OnCultureChanged()
{
	RefreshOptions();
}
