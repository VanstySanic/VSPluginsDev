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
#include "Libraries/VSSetContainerLibrary.h"

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, DefaultOptions, TArray<FString>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonRotator, TextLabels, TArray<FText>);

UVSSettingItemBase_Integer::UVSSettingItemBase_Integer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItemBase_Integer::PostInitProperties()
{
	Super::PostInitProperties();
	RefreshOptions();
}

#if WITH_EDITOR
void UVSSettingItemBase_Integer::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, bUseGeneratedOptions)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, bOptionsHighToLow)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, CustomOptions)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, DesiredValueRange)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, DesiredValueStep)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, bEnableLowerBoundExtensionReroute)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, bEnableUpperBoundExtensionReroute)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, LowerBoundExtensionRerouteValue)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Integer, UpperBoundExtensionRerouteValue))
	{
		RefreshOptions();
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemBase_Integer::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	RefreshOptions();
}

void UVSSettingItemBase_Integer::Validate_Implementation()
{
	Super::Validate_Implementation();

	const int32 SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	int32 ValidateValue = GetConditionToSettingValue(SettingValue);

	if (ValidateValue != SettingValue)
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
			Slider->SetValue(GetSettingToConditionValue(SettingValue));
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
		TArray<int32> Options = bUseGeneratedOptions ? CalcGeneratedOptions() : CustomOptions;
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

			Slider->SetValue(GetSettingToConditionValue(SettingValue));
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

TArray<int32> UVSSettingItemBase_Integer::GetOptions() const
{
	return bUseGeneratedOptions ? GeneratedOptions : CustomOptions;
}

void UVSSettingItemBase_Integer::RefreshOptions()
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
	TArray<int32> Options = bUseGeneratedOptions ? GeneratedOptions : CustomOptions;
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
	

	/** Must rebind the value widgets after the options are changed. */
	const TArray<UWidget*>& ValueWidgets = GetBoundWidgetsOfType(FName("Value"));
	for (UWidget* BoundWidget : ValueWidgets)
	{
		RebindWidget(BoundWidget);
	}
}

int32 UVSSettingItemBase_Integer::GetConditionToSettingValue(const int32 InValue)
{
	int32 OutValue = InValue;
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
		const TArray<int32> Options = GetOptions();
		int32 Index = UVSSetContainerLibrary::GetArrayNearestElementIndex<int32>(InValue, Options);
		if (Options.IsValidIndex(Index))
		{
			OutValue = Options[Index];
		}
	}

	return OutValue;
}

int32 UVSSettingItemBase_Integer::GetSettingToConditionValue(const int32 InValue)
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

TArray<int32> UVSSettingItemBase_Integer::CalcGeneratedOptions_Implementation() const
{
	/** Generate options. */
	TArray<int32> Options;
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

void UVSSettingItemBase_Integer::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	for (int32 Option : GetOptions())
	{
		if (SelectedItem == GetValueContentText(Option).ToString())
		{
			SetValue(Option);
			break;
		}
	}
}

void UVSSettingItemBase_Integer::OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir)
{
	const TArray<int32> Options = GetOptions();
	const int32 NewValToSet = Options.IsValidIndex(NewValue) ? Options[NewValue] : NewValue;
	SetValue(NewValToSet);
}

void UVSSettingItemBase_Integer::OnValueSliderValueChanged(float NewValue)
{
	const int32 SettingValue = GetValue(EVSSettingItemValueSource::Settings);
	const int32 NewIntValue = FMath::RoundToInt(NewValue);

	if (SettingValue == NewIntValue) return;

	const TArray<int32> Options = GetOptions();
	const int32 SettingOptionIndex = UVSSetContainerLibrary::GetArrayNearestElementIndex<int32>(SettingValue, Options);

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			const int32 SliderValue = FMath::RoundToInt(Slider->GetValue());
			
			/** Not set by mouse movement. */
			if (!Slider->HasMouseCapture())
			{
				/** Is it the slider that has been set value. */
				if (SliderValue == NewIntValue)
				{
					/** The value set is within one step. */
					if (FMath::IsNearlyEqual(SettingValue, SliderValue, DesiredValueStep * 1.1f))
					{
						/** Snap value to option. */
						const int32 NewValueIndex = SettingOptionIndex + (NewIntValue > SettingValue ? 1 : -1);
						if (Options.IsValidIndex(NewIntValue))
						{
							SetValue(GetConditionToSettingValue(Options[NewValueIndex]));
							return;
						}
					}
				}
			}
		}
	}
	
	SetValue(GetConditionToSettingValue(FMath::RoundToInt(NewValue)));
}