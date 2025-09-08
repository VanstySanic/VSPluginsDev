// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSSettingItemBase_Boolean.h"
#include "VSPrivablic.h"
#include "VSSettingSystemConfig.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Libraries/VSGameplayLibrary.h"

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, DefaultOptions, TArray<FString>);

UVSSettingItemBase_Boolean::UVSSettingItemBase_Boolean(const FObjectInitializer& ObjectInitializer)
{
	NamedBooleans = UVSSettingSystemConfig::Get()->EnableDisableTexts;
}

#if WITH_EDITOR
void UVSSettingItemBase_Boolean::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItemBase_Boolean, bOptionsTrueThenFalse))
	{
		TArray<UWidget*> ValueWidgets = GetBoundWidgetsOfType(FName("Value"));
		for (UWidget* Widget : ValueWidgets)
		{
			RebindWidget(Widget);
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItemBase_Boolean::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItemBase_Boolean::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetValue(EVSSettingItemValueSource::Settings) == GetValue(ValueSource);
}

void UVSSettingItemBase_Boolean::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	const bool bIsInGame = UVSGameplayLibrary::IsInGame();
	
	/** Update widget values. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (bIsInGame) { ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_Boolean::OnValueComboBoxStringSelectionChanged); }
			ComboBox->SetSelectedOption(NamedBooleans.FindRef(GetValue(EVSSettingItemValueSource::Settings)).ToString());
			if (bIsInGame) { ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItemBase_Boolean::OnValueComboBoxStringSelectionChanged); }
		}
		else if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			if (bIsInGame) { CheckBox->OnCheckStateChanged.RemoveDynamic(this, &UVSSettingItemBase_Boolean::UVSSettingItemBase_Boolean::OnValueCheckBoxCheckStateChanged); }
			CheckBox->SetIsChecked(GetValue(EVSSettingItemValueSource::Settings));
			if (bIsInGame) { CheckBox->OnCheckStateChanged.AddDynamic(this, &UVSSettingItemBase_Boolean::UVSSettingItemBase_Boolean::OnValueCheckBoxCheckStateChanged); }
		}
	}

	/** Refresh content. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(NamedBooleans.FindRef(GetValue(EVSSettingItemValueSource::Settings)));
		}
	}
}

void UVSSettingItemBase_Boolean::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	Super::BindWidgetInternal_Implementation(Widget, TypeName);

	if (TypeName == FName(TEXT("Value")))
	{
		/** Init options. */
		TArray<bool> Options = bOptionsTrueThenFalse ? TArray<bool>{ true, false } : TArray<bool>{ false , true };
		
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (!UVSGameplayLibrary::IsInGame())
			{
				VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Empty();
				for (bool Option : Options)
				{
					VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Add(NamedBooleans.FindRef(Option).ToString());
				}
			}
			else
			{
				ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItemBase_Boolean::OnValueComboBoxStringSelectionChanged);
			}
			
			ComboBox->ClearOptions();
			for (bool Option : Options)
			{
				ComboBox->AddOption(NamedBooleans.FindRef(Option).ToString());
			}

			ComboBox->RefreshOptions();
			ComboBox->SetSelectedIndex(GetValue(EVSSettingItemValueSource::Settings));
		}
		else if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				CheckBox->OnCheckStateChanged.AddDynamic(this, &UVSSettingItemBase_Boolean::UVSSettingItemBase_Boolean::OnValueCheckBoxCheckStateChanged);
			}
			CheckBox->SetIsChecked(GetValue(EVSSettingItemValueSource::Settings));
		}
	}
	else if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(NamedBooleans.FindRef(GetValue(EVSSettingItemValueSource::Settings)));
		}
	}
}

void UVSSettingItemBase_Boolean::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Value"))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItemBase_Boolean::OnValueComboBoxStringSelectionChanged);
			}
		}
		else if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				CheckBox->OnCheckStateChanged.RemoveDynamic(this, &UVSSettingItemBase_Boolean::UVSSettingItemBase_Boolean::OnValueCheckBoxCheckStateChanged);
			}
		}
	}
	
	Super::UnbindWidgetInternal_Implementation(Widget, TypeName);
}

void UVSSettingItemBase_Boolean::SetValue_Implementation(bool bInValue)
{
}

bool UVSSettingItemBase_Boolean::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return false;
}

void UVSSettingItemBase_Boolean::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	for (const TPair<bool, FText>& NamedBoolean : NamedBooleans)
	{
		if (SelectedItem == NamedBoolean.Value.ToString())
		{
			SetValue(NamedBoolean.Key);
			break;
		}
	}
}

void UVSSettingItemBase_Boolean::OnValueCheckBoxCheckStateChanged(bool bIsChecked)
{
	if (NamedBooleans.Contains(bIsChecked))
	{
		SetValue(bIsChecked);
	}
}
