// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemBase_Float.h"
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

UVSSettingItemBase_Float::UVSSettingItemBase_Float(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer)
{
}

void UVSSettingItemBase_Float::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (DesiredValueRange.X > DesiredValueRange.Y)
	{
		Swap(DesiredValueRange.X, DesiredValueRange.Y);
	}

	float ValidateValue = GetValue(EVSSettingItemValueSource::Settings);

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
		else if (!bUseGeneratedOptions)
		{
			for (float CustomValue : CustomOptions)
			{
				if (UKismetMathLibrary::InRange_FloatFloat(CustomValue, ValidateValue - DesiredValueStep * 0.5f, ValidateValue + DesiredValueStep * 0.5f))
				{
					bShouldClampRange = false;
					break;
				}
			}
		}
		else
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

void UVSSettingItemBase_Float::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItemBase_Float::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetValue(EVSSettingItemValueSource::Settings) == GetValue(ValueSource);
}

void UVSSettingItemBase_Float::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	const bool bIsInGame = UVSGameplayLibrary::IsInGame();
	const float SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const FText& SettingText = GetValueContentText(SettingValue);
	
	/** Update widget values. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (bIsInGame) { ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_Float::OnValueComboBoxStringSelectionChanged); }
			ComboBox->SetSelectedOption(SettingText.ToString());
			if (bIsInGame) { ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItemBase_Float::OnValueComboBoxStringSelectionChanged); }
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (bIsInGame) { Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItemBase_Float::OnValueRotatorRotatedWithDirection); }
			for (int i = 0; i < VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels).Num(); i++)
			{
				if (SettingText.EqualTo(VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels)[i]))
				{
					Rotator->SetSelectedItem(i);
					break;
				}
			}
			if (bIsInGame) { Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_Float::OnValueRotatorRotatedWithDirection); }
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			if (bIsInGame) { Slider->OnValueChanged.RemoveDynamic(this, &UVSSettingItemBase_Float::OnValueSliderValueChanged); }
			
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
			
			if (bIsInGame) { Slider->OnValueChanged.AddDynamic(this, &UVSSettingItemBase_Float::OnValueSliderValueChanged); }
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

void UVSSettingItemBase_Float::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	Super::BindWidgetInternal_Implementation(Widget, TypeName);
	
	const float SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const FText& SettingText = GetValueContentText(SettingValue);

	if (TypeName == FName("Value"))
	{
		/** Generate options. */
		TArray<float> Options = bUseGeneratedOptions ? GenerateOptions() : CustomOptions;
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
				for (const FText& Option : OptionTexts)
				{
					VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Add(Option.ToString());
				}
			}
			else
			{
				ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &UVSSettingItemBase_Float::OnValueComboBoxStringSelectionChanged);
			}
			
			ComboBox->ClearOptions();
			for (const FText& OptionText : OptionTexts)
			{
				ComboBox->AddOption(OptionText.ToString());
			}
			
			ComboBox->RefreshOptions();
			ComboBox->SetSelectedOption(SettingText.ToString());
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_Float::OnValueRotatorRotatedWithDirection);
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
				Slider->OnValueChanged.AddDynamic(this, &UVSSettingItemBase_Float::OnValueSliderValueChanged);
			}

			Slider->MouseUsesStep = true;
			Slider->SetMinValue(DesiredValueRange.X - (bEnableLowerBoundExtensionReroute ? DesiredValueStep : 0.f));
			Slider->SetMaxValue(DesiredValueRange.Y + (bEnableUpperBoundExtensionReroute ? DesiredValueStep : 0.f));
			Slider->SetStepSize(DesiredValueStep);

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
	else if (TypeName.IsEqual(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItemBase_Float::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Value"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Slider->OnValueChanged.RemoveDynamic(this, &UVSSettingItemBase_Float::OnValueSliderValueChanged);
			}
		}
	}
	
	Super::UnbindWidgetInternal_Implementation(Widget, TypeName);
}

void UVSSettingItemBase_Float::SetValue_Implementation(float InValue)
{
}

float UVSSettingItemBase_Float::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return 0.f;
}

FText UVSSettingItemBase_Float::GetValueContentText_Implementation(float InValue) const
{
	for (const TPair<float, FText>& NamedFloat : NamedValues)
	{
		if (InValue >= NamedFloat.Key - DesiredValueStep / 2.f
			&& InValue <= NamedFloat.Key + DesiredValueStep / 2.f)
		{
			return NamedFloat.Value;
		}
	}
	
	const FText& ValueText = UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 324, DesiredFractionNum, DesiredFractionNum);
	const FText& FormatText = FText::Format(ContentTextFormat, ValueText);
	return FormatText;
}

TArray<float> UVSSettingItemBase_Float::GenerateOptions_Implementation() const
{
	TArray<float> Options;
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

void UVSSettingItemBase_Float::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bUseGeneratedOptions)
	{
		if (!bGenerateOptionsHighToLow)
		{
			for (float i = DesiredValueRange.X; i <= DesiredValueRange.Y + DesiredValueStep / 2.f; i+= DesiredValueStep)
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
			for (float i = DesiredValueRange.Y; i >= DesiredValueRange.X - DesiredValueStep / 2.f; i -= DesiredValueStep)
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
		for (float CustomOption : CustomOptions)
		{
			if (SelectedItem == GetValueContentText(CustomOption).ToString())
			{
				SetValue(CustomOption);
				break;
			}
		}
	}
}

void UVSSettingItemBase_Float::OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir)
{
	float NewValToSet = NewValue;

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

void UVSSettingItemBase_Float::OnValueSliderValueChanged(float NewValue)
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
		SetValue(NewValue);
	}
}
