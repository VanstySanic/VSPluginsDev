// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemBase_String.h"
#include "CommonRotator.h"
#include "CommonTextBlock.h"
#include "VSPrivablic.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Libraries/VSGameplayLibrary.h"

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, DefaultOptions, TArray<FString>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonRotator, TextLabels, TArray<FText>);

UVSSettingItemBase_String::UVSSettingItemBase_String(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
}

void UVSSettingItemBase_String::PostInitProperties()
{
	Super::PostInitProperties();
	RefreshOptions();
}

#if WITH_EDITOR
void UVSSettingItemBase_String::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_String, bUseGeneratedOptions)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_String, CustomOptions)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_String, bSortOptionsByName)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_String, bSortOptionsHighToLow))
	{
		RefreshOptions();
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_String, NamedValues))
	{
		RebindWidgetsOfType(FName("Value"));
		RebindWidgetsOfType(FName("Content"));
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemBase_String::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	RefreshOptions();
}

void UVSSettingItemBase_String::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(EVSSettingItemValueSource::Settings));
}

bool UVSSettingItemBase_String::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetValue(EVSSettingItemValueSource::Settings) == GetValue(ValueSource);
}

void UVSSettingItemBase_String::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	const bool bIsInGame = UVSGameplayLibrary::IsInGame();
	const FString& SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const FText& SettingText = GetValueContentText(SettingValue);
	
	/** Update widget values. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (bIsInGame) { ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_String::OnValueComboBoxStringSelectionChanged); }
			ComboBox->SetSelectedOption(SettingText.ToString());
			if (bIsInGame) { ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItemBase_String::OnValueComboBoxStringSelectionChanged); }
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (bIsInGame) { Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItemBase_String::OnValueRotatorRotatedWithDirection); }
			for (int i = 0; i < VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels).Num(); i++)
			{
				if (SettingText.EqualTo(VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels)[i]))
				{
					Rotator->SetSelectedItem(i);
					break;
				}
			}
			if (bIsInGame) { Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_String::OnValueRotatorRotatedWithDirection); }
		}
	}

	/** Refresh content. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItemBase_String::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	Super::BindWidgetInternal_Implementation(Widget, TypeName);

	const FString& SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const FText& SettingText = GetValueContentText(SettingValue);
	
	if (TypeName == FName("Value"))
	{
		/** Generate options. */
		TArray<FString> Options = GetOptions();
		TArray<FText> OptionTexts;
		for (const FString& Option : Options)
		{
			OptionTexts.Add(GetValueContentText(Option));
		}
		
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			ComboBox->ClearOptions();
			for (const FText& Option : OptionTexts)
			{
				ComboBox->AddOption(Option.ToString());
			}
			ComboBox->RefreshOptions();
			ComboBox->SetSelectedOption(SettingText.ToString());
			
			if (!UVSGameplayLibrary::IsInGame())
			{
				VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Empty();
				for (const FText& OptionText : OptionTexts)
				{
					VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Add(OptionText.ToString());
				}
			}
			else
			{
				ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &UVSSettingItemBase_String::OnValueComboBoxStringSelectionChanged);
			}
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			Rotator->PopulateTextLabels(OptionTexts);

			/** Init the original item. */
			for (int i = 0; i < OptionTexts.Num(); ++i)
			{
				if (SettingText.EqualTo(OptionTexts[i]))
				{
					Rotator->SetSelectedItem(i);
					break;
				}
			}

			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_String::OnValueRotatorRotatedWithDirection);
			}
		}
	}
	else if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItemBase_String::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Value"))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_String::OnValueComboBoxStringSelectionChanged);
			}
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItemBase_String::OnValueRotatorRotatedWithDirection);
			}
		}
	}
	
	Super::UnbindWidgetInternal_Implementation(Widget, TypeName);
}

TArray<FString> UVSSettingItemBase_String::CalcGeneratedOptions_Implementation() const
{
	return TArray<FString>();
}

FText UVSSettingItemBase_String::GetValueContentText_Implementation(const FString& InValue) const
{
	if (NamedValues.Contains(InValue))
	{
		return NamedValues.FindRef(InValue);
	}

	return FText::FromString(InValue);
}

TArray<FString> UVSSettingItemBase_String::GetOptions() const
{
	return bUseGeneratedOptions ? GeneratedOptions : CustomOptions;
}

void UVSSettingItemBase_String::RefreshOptions()
{
	if (bUseGeneratedOptions)
	{
		GeneratedOptions = CalcGeneratedOptions();
	}
	else
	{
		GeneratedOptions.Empty();
	}
	
	if (bSortOptionsByName)
	{
		TArray<FString> Options = bUseGeneratedOptions ? GeneratedOptions : CustomOptions;
		Options.Sort([&](const FString& A, const FString& B)
		{
			if (!bSortOptionsByName)
			{
				return A < B;
			}
			return GetValueContentText(A).ToString() < GetValueContentText(B).ToString();
		});

		if (bSortOptionsHighToLow)
		{
			Algo::Reverse(Options);
		}

		if (bUseGeneratedOptions)
		{
			GeneratedOptions = Options;
		}
		else
		{
			CustomOptions = Options;
		}
	}
	
	/** Must rebind the value widgets after the options are changed. */
	RebindWidgetsOfType(FName("Value"));
}

void UVSSettingItemBase_String::SetValue_Implementation(const FString& InValue)
{
}

FString UVSSettingItemBase_String::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return FString();
}

void UVSSettingItemBase_String::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	for (const FString& Option : GetOptions())
	{
		if (SelectedItem == GetValueContentText(Option).ToString())
		{
			SetValue(Option);
			break;
		}
	}
}

void UVSSettingItemBase_String::OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir)
{
	const TArray<FString> Options = GetOptions();
	const FString& NewValToSet = Options.IsValidIndex(NewValue) ? Options[NewValue] : FString();
	SetValue(NewValToSet);	
}
