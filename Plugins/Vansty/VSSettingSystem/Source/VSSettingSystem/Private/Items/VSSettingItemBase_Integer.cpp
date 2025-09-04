// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemBase_Integer.h"
#include "CommonRotator.h"
#include "VSPrivablic.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Libraries/VSGameplayLibrary.h"

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, DefaultOptions, TArray<FString>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonRotator, TextLabels, TArray<FText>);

UVSSettingItemBase_Integer::UVSSettingItemBase_Integer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItemBase_Integer::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (DesiredValueRange.X > DesiredValueRange.Y)
	{
		Swap(DesiredValueRange.X, DesiredValueRange.Y);
	}

	int32 ValidateValue = GetValue(EVSSettingItemValueSource::Settings);

	bool bClampBorderExtension = false;
	if (bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(ValidateValue, DesiredValueRange.X - DesiredValueStep, DesiredValueStep / 2.f))
	{
		ValidateValue = LowerBoundExtensionRerouteValue;
		bClampBorderExtension = true;
	}
	else if (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(ValidateValue, DesiredValueRange.Y + DesiredValueStep, DesiredValueStep / 2.f))
	{
		ValidateValue = UpperBoundExtensionRerouteValue;
		bClampBorderExtension = true;
	}
	
	if (bClampBorderExtension)
	{
		SetValue(ValidateValue);
	}
	else if (!UKismetMathLibrary::InRange_FloatFloat(ValidateValue, DesiredValueRange.X, DesiredValueRange.Y))
	{
		bool bShouldClampRange = true;
		if ((bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(ValidateValue, LowerBoundExtensionRerouteValue, DesiredValueStep / 2.f))
			|| (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(ValidateValue, UpperBoundExtensionRerouteValue, DesiredValueStep / 2.f)))
		{
			bShouldClampRange = false;
		}
		else if (bUseGeneratedOptions || CustomOptions.Contains(ValidateValue))
		{
			bShouldClampRange = false;
		}

		if (bShouldClampRange)
		{
			ValidateValue = FMath::Clamp(ValidateValue, DesiredValueRange.X, DesiredValueRange.Y);
			SetValue(ValidateValue);
		}
	}
}

void UVSSettingItemBase_Integer::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItemBase_Integer::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetValue(EVSSettingItemValueSource::Settings) == GetValue(ValueSource);
}

void UVSSettingItemBase_Integer::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	const bool bIsInGame = UVSGameplayLibrary::IsInGame();
	const int32 SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const FText& SettingText = GetValueContentText(SettingValue);
	
	/** Update widget values. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (bIsInGame) { ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_Integer::OnValueComboBoxStringSelectionChanged); }
			ComboBox->SetSelectedOption(SettingText.ToString());
			if (bIsInGame) { ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItemBase_Integer::OnValueComboBoxStringSelectionChanged); }
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (bIsInGame) { Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItemBase_Integer::OnValueRotatorRotatedWithDirection); }
			for (int i = 0; i < VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels).Num(); i++)
			{
				if (SettingText.EqualTo(VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels)[i]))
				{
					Rotator->SetSelectedItem(i);
					break;
				}
			}
			if (bIsInGame) { Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_Integer::OnValueRotatorRotatedWithDirection); }
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			if (bIsInGame) { Slider->OnValueChanged.RemoveDynamic(this, &UVSSettingItemBase_Integer::OnValueSliderValueChanged); }

			if (bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(SettingValue, LowerBoundExtensionRerouteValue, DesiredValueStep / 2.f))
			{
				Slider->SetValue(DesiredValueRange.X - 1.f);
			}
			else if (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(SettingValue, UpperBoundExtensionRerouteValue, DesiredValueStep / 2.f))
			{
				Slider->SetValue(DesiredValueRange.Y + 1.f);
			}
			else
			{
				Slider->SetValue(SettingValue);
			}
			
			if (bIsInGame) { Slider->OnValueChanged.AddDynamic(this, &UVSSettingItemBase_Integer::OnValueSliderValueChanged); }
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

void UVSSettingItemBase_Integer::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	Super::BindWidgetInternal_Implementation(Widget, TypeName);

	const int32 SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const FText& SettingText = GetValueContentText(SettingValue);
	
	if (TypeName == FName("Value"))
	{
		/** Generate options. */
		TArray<int32> Options = bUseGeneratedOptions ? GenerateOptions() : CustomOptions;
		TArray<FText> OptionTexts;
		for (int32 Option : Options)
		{
			OptionTexts.Add(GetValueContentText(Option));
		}
		
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
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
				ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &UVSSettingItemBase_Integer::OnValueComboBoxStringSelectionChanged);
			}
			
			ComboBox->ClearOptions();
			for (const FText& Option : OptionTexts)
			{
				ComboBox->AddOption(Option.ToString());
			}
			
			ComboBox->RefreshOptions();
			ComboBox->SetSelectedOption(SettingText.ToString());
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_Integer::OnValueRotatorRotatedWithDirection);
			}
		
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
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Slider->OnValueChanged.AddDynamic(this, &UVSSettingItemBase_Integer::OnValueSliderValueChanged);
			}

			Slider->MouseUsesStep = true;
			Slider->SetMinValue(DesiredValueRange.X - (bEnableLowerBoundExtensionReroute ? DesiredValueStep : 0.f));
			Slider->SetMaxValue(DesiredValueRange.Y + (bEnableUpperBoundExtensionReroute ? DesiredValueStep : 0.f));
			Slider->SetStepSize(DesiredValueStep);
			Slider->SetValue(GetValue(EVSSettingItemValueSource::Settings));

			if (bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(SettingValue, LowerBoundExtensionRerouteValue, DesiredValueStep / 2.f))
			{
				Slider->SetValue(DesiredValueRange.X - DesiredValueStep);
			}
			else if (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(SettingValue, UpperBoundExtensionRerouteValue, DesiredValueStep / 2.f))
			{
				Slider->SetValue(DesiredValueRange.Y + DesiredValueStep);
			}
			else
			{
				Slider->SetValue(SettingValue);
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

void UVSSettingItemBase_Integer::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Value"))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_Integer::OnValueComboBoxStringSelectionChanged);
			}
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItemBase_Integer::OnValueRotatorRotatedWithDirection);
			}
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Slider->OnValueChanged.RemoveDynamic(this, &UVSSettingItemBase_Integer::OnValueSliderValueChanged);
			}
		}
	}
	
	Super::UnbindWidgetInternal_Implementation(Widget, TypeName);
}

void UVSSettingItemBase_Integer::SetValue_Implementation(int32 InValue)
{
}

int32 UVSSettingItemBase_Integer::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return 0;
}

FText UVSSettingItemBase_Integer::GetValueContentText(int32 InValue) const
{
	if (NamedValues.Contains(InValue))
	{
		return NamedValues.FindRef(InValue);
	}
	
	const FText& ValueText = UKismetTextLibrary::Conv_IntToText(InValue);
	const FText& FormatText = FText::Format(ContentTextFormat, ValueText);
	return FormatText;
}

TArray<int32> UVSSettingItemBase_Integer::GenerateOptions_Implementation() const
{
	/** Generate options. */
	TArray<int32> Options;
	if (!bGenerateOptionsHighToLow)
	{
		for (int i = DesiredValueRange.X; i <= DesiredValueRange.Y + DesiredValueStep - 1; i += DesiredValueStep)
		{
			i = FMath::Clamp(i, DesiredValueRange.X, DesiredValueRange.Y);
			Options.Add(i);
		}
	}
	else
	{
		for (int i = DesiredValueRange.Y; i >= DesiredValueRange.X - DesiredValueStep + 1; i -= DesiredValueStep)
		{
			i = FMath::Clamp(i, DesiredValueRange.X, DesiredValueRange.Y);
			Options.Add(i);
		}
	}
	return Options;
}

void UVSSettingItemBase_Integer::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bUseGeneratedOptions)
	{
		if (!bGenerateOptionsHighToLow)
		{
			for (int i = DesiredValueRange.X; i <= DesiredValueRange.Y + DesiredValueStep - 1; i+= DesiredValueStep)
			{
				i = FMath::Clamp(i, DesiredValueRange.X, DesiredValueRange.Y);
				if (SelectedItem == GetValueContentText(i).ToString())
				{
					SetValue(i);
					break;
				}
			}
		}
		else
		{
			for (int i = DesiredValueRange.Y; i >= DesiredValueRange.X - DesiredValueStep + 1; i -= DesiredValueStep)
			{
				i = FMath::Clamp(i, DesiredValueRange.X, DesiredValueRange.Y);
				if (SelectedItem == GetValueContentText(i).ToString())
				{
					SetValue(i);
					break;
				}
			}
		}
	}
	else
	{
		for (int32 CustomOption : CustomOptions)
		{
			if (SelectedItem == GetValueContentText(CustomOption).ToString())
			{
				SetValue(CustomOption);
				break;
			}
		}
	}
}

void UVSSettingItemBase_Integer::OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir)
{
	int32 NewValToSet = NewValue;

	if (bUseGeneratedOptions)
	{
		if (!bGenerateOptionsHighToLow)
		{
			NewValToSet = DesiredValueRange.X + DesiredValueStep * NewValue;
		}
		else
		{
			NewValToSet = DesiredValueRange.Y - DesiredValueStep * NewValue;
		}
		NewValToSet = FMath::Clamp(NewValue, DesiredValueRange.X, DesiredValueRange.Y);
	}
	else
	{
		NewValToSet = CustomOptions.IsValidIndex(NewValue) ? CustomOptions[NewValue] : NewValue;
	}
	
	SetValue(NewValToSet);
}

void UVSSettingItemBase_Integer::OnValueSliderValueChanged(float NewValue)
{
	if (bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(NewValue, DesiredValueRange.X - DesiredValueStep, DesiredValueStep / 2.f))
	{
		SetValue(LowerBoundExtensionRerouteValue);
	}
	else if (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(NewValue, DesiredValueRange.Y + DesiredValueStep, DesiredValueStep / 2.f))
	{
		SetValue(UpperBoundExtensionRerouteValue);
	}
	else
	{
		SetValue(FMath::RoundToInt(NewValue));
	}
}