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
#include "Libraries/VSSetContainerLibrary.h"

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, DefaultOptions, TArray<FString>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonRotator, TextLabels, TArray<FText>);

UVSSettingItemBase_Float::UVSSettingItemBase_Float(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer)
{
}

void UVSSettingItemBase_Float::PostInitProperties()
{
	Super::PostInitProperties();
	RefreshOptions();
}

#if WITH_EDITOR
void UVSSettingItemBase_Float::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, bUseGeneratedOptions)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, bOptionsHighToLow)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, CustomOptions)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, DesiredValueRange)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, DesiredValueStep)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, bEnableLowerBoundExtensionReroute)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, bEnableUpperBoundExtensionReroute)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, LowerBoundExtensionRerouteValue)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Float, UpperBoundExtensionRerouteValue))
	{
		RefreshOptions();
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemBase_Float::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	RefreshOptions();
}

void UVSSettingItemBase_Float::Validate_Implementation()
{
	const float SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	float ValidateValue = GetConditionToSettingValue(SettingValue);

	if (!FMath::IsNearlyEqual(ValidateValue, SettingValue))
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
		else
		{
			for (float Option : GetOptions())
			{
				if (UKismetMathLibrary::InRange_FloatFloat(Option, ValidateValue - DesiredValueStep * 0.5f, ValidateValue + DesiredValueStep * 0.5f))
				{
					bShouldClampRange = false;
					break;
				}
			}
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
			Slider->SetValue(GetSettingToConditionValue(SettingValue));
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
		const TArray<float>& Options = GetOptions();
		TArray<FText> OptionTexts;
		for (float Option : Options)
		{
			OptionTexts.Add(GetValueContentText(Option));
		}
		
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			ComboBox->ClearOptions();
			for (const FText& OptionText : OptionTexts)
			{
				ComboBox->AddOption(OptionText.ToString());
			}
			
			ComboBox->RefreshOptions();
			ComboBox->SetSelectedOption(SettingText.ToString());

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
				Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItemBase_Float::OnValueRotatorRotatedWithDirection);
			}
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->MouseUsesStep = true;
			Slider->SetMinValue(DesiredValueRange.X - (bEnableLowerBoundExtensionReroute ? DesiredValueStep : 0.f));
			Slider->SetMaxValue(DesiredValueRange.Y + (bEnableUpperBoundExtensionReroute ? DesiredValueStep : 0.f));
			Slider->SetStepSize(DesiredValueStep);

			Slider->SetValue(GetSettingToConditionValue(SettingValue));

			if (UVSGameplayLibrary::IsInGame())
			{
				Slider->OnValueChanged.AddDynamic(this, &UVSSettingItemBase_Float::OnValueSliderValueChanged);
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
	
	const FText& ValueText = UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 324, DisplayFractionNum, DisplayFractionNum);
	const FText& FormatText = FText::Format(ContentTextFormat, ValueText);
	return FormatText;
}

TArray<float> UVSSettingItemBase_Float::CalcGeneratedOptions_Implementation() const
{
	TArray<float> Options;
	if (!bOptionsHighToLow)
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

	if (bEnableLowerBoundExtensionReroute)
	{
		if (!bOptionsHighToLow)
		{
			Options.Insert(LowerBoundExtensionRerouteValue, 0);
		}
		else
		{
			Options.Add(LowerBoundExtensionRerouteValue);
		}
	}

	if (bEnableUpperBoundExtensionReroute)
	{
		if (!bOptionsHighToLow)
		{
			Options.Add(UpperBoundExtensionRerouteValue);
		}
		else
		{
			Options.Insert(UpperBoundExtensionRerouteValue, 0);
		}
	}
	
	return Options;
}

TArray<float> UVSSettingItemBase_Float::GetOptions() const
{
	return bUseGeneratedOptions ? GeneratedOptions : CustomOptions;
}

void UVSSettingItemBase_Float::RefreshOptions()
{
	if (bUseGeneratedOptions)
	{
		GeneratedOptions = CalcGeneratedOptions();
	}
	else
	{
		GeneratedOptions.Empty();
	}

	/** Sort options to the right order. */
	TArray<float> Options = bUseGeneratedOptions ? GeneratedOptions : CustomOptions;
	Algo::Sort(Options);
	if (bEnableLowerBoundExtensionReroute)
	{
		Options.RemoveSingle(LowerBoundExtensionRerouteValue);
		Options.Insert(LowerBoundExtensionRerouteValue, 0);
	}
	if (bEnableUpperBoundExtensionReroute)
	{
		Options.RemoveSingle(LowerBoundExtensionRerouteValue);
		Options.Push(LowerBoundExtensionRerouteValue);
	}
	if (bOptionsHighToLow)
	{
		Algo::Reverse(Options);
	}
	

	/** Must rebind the value widgets. */
	const TArray<UWidget*>& ValueWidgets = GetBoundWidgetsOfType(FName("Value"));
	for (UWidget* BoundWidget : ValueWidgets)
	{
		RebindWidget(BoundWidget);
	}
}

float UVSSettingItemBase_Float::GetConditionToSettingValue(const float InValue)
{
	float OutValue = InValue;
	if (bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(InValue, DesiredValueRange.X - DesiredValueStep, DesiredValueStep / 2.f))
	{
		OutValue = LowerBoundExtensionRerouteValue;
	}
	else if (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(InValue, DesiredValueRange.Y + DesiredValueStep, DesiredValueStep / 2.f))
	{
		OutValue = UpperBoundExtensionRerouteValue;
	}
	else
	{
		/** Round to options. */
		const TArray<float> Options = GetOptions();
		int32 Index = UVSSetContainerLibrary::GetArrayNearestElementIndex<float>(InValue, Options);
		if (Options.IsValidIndex(Index))
		{
			OutValue = Options[Index];
		}
	}

	return OutValue;
}

float UVSSettingItemBase_Float::GetSettingToConditionValue(const float InValue)
{
	float OutValue = InValue;
	if (bEnableLowerBoundExtensionReroute && FMath::IsNearlyEqual(InValue, LowerBoundExtensionRerouteValue, DesiredValueStep / 2.f))
	{
		OutValue = DesiredValueRange.X - DesiredValueStep;
	}
	else if (bEnableUpperBoundExtensionReroute && FMath::IsNearlyEqual(InValue, UpperBoundExtensionRerouteValue, DesiredValueStep / 2.f))
	{
		OutValue = DesiredValueRange.Y + DesiredValueStep;
	}
	return OutValue;
}

void UVSSettingItemBase_Float::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	for (float Option : GetOptions())
	{
		if (SelectedItem == GetValueContentText(Option).ToString())
		{
			SetValue(Option);
			break;
		}
	}
}

void UVSSettingItemBase_Float::OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir)
{
	const TArray<float>& Options = GetOptions();
	const float NewValToSet = Options.IsValidIndex(NewValue) ? Options[NewValue] : NewValue;
	SetValue(NewValToSet);
}

void UVSSettingItemBase_Float::OnValueSliderValueChanged(float NewValue)
{
	const float SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	if (FMath::IsNearlyEqual(SettingValue, NewValue)) return;

	const TArray<float> Options = GetOptions();
	const int32 SettingOptionIndex = UVSSetContainerLibrary::GetArrayNearestElementIndex(SettingValue, Options);

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			const float SliderValue = Slider->GetValue();
			
			/** Not set by mouse movement. */
			if (!Slider->HasMouseCapture())
			{
				/** Is it the slider that has been set value. */
				if (FMath::IsNearlyEqual(SliderValue, NewValue))
				{
					/** The value set is within one step. */
					if (FMath::IsNearlyEqual(SettingValue, SliderValue, DesiredValueStep * 1.1f))
					{
						/** Snap the value to option. */
						const int32 NewValueIndex = SettingOptionIndex + (NewValue > SettingValue ? 1 : -1);
						if (Options.IsValidIndex(NewValueIndex))
						{
							SetValue(GetConditionToSettingValue(Options[NewValueIndex]));
							return;
						}
					}
				}
			}
		}
	}

	SetValue(GetConditionToSettingValue(NewValue));
}
