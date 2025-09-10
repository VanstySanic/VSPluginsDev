// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_Integer.h"
#include "CommonRotator.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

UVSSettingItem_ConsoleVariable_Integer::UVSSettingItem_ConsoleVariable_Integer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ConsoleVariable_Integer::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
	check(ConsoleVariable);

	OnVariableChangedDelegateHandle = ConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSSettingItem_ConsoleVariable_Integer::OnConsoleVariableChanged);
}

void UVSSettingItem_ConsoleVariable_Integer::Uninitialize_Implementation()
{
	if (OnVariableChangedDelegateHandle.IsValid())
	{
		ConsoleVariable->OnChangedDelegate().Remove(OnVariableChangedDelegateHandle);
	}

	Super::Uninitialize_Implementation();
}

void UVSSettingItem_ConsoleVariable_Integer::Load_Implementation()
{
	int32 LoadedValue = GetValue(EVSSettingItemValueSource::Settings);
	GConfig->GetInt(TEXT("ConsoleVariable"), *(TEXT("Bool.") + ConsoleVariableName), LoadedValue, GGameUserSettingsIni);
	SetValue(LoadedValue);
}

void UVSSettingItem_ConsoleVariable_Integer::Apply_Implementation()
{
	GetConsoleVariable()->Set(Value);
}

void UVSSettingItem_ConsoleVariable_Integer::Confirm_Implementation()
{
	LastConfirmedValue = Value;
}

void UVSSettingItem_ConsoleVariable_Integer::Save_Implementation()
{
	GConfig->SetInt(TEXT("ConsoleVariable"), *(TEXT("Int.") + ConsoleVariableName), Value, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Integer::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItem_ConsoleVariable_Integer::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return Value == GetValue(ValueSource);
}

void UVSSettingItem_ConsoleVariable_Integer::OnItemValueUpdated_Implementation()
{
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			ComboBox->SetSelectedOption(GetValueContentText(GetValue(EVSSettingItemValueSource::Settings)).ToString());
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			Rotator->SetSelectedItem(GetValue(EVSSettingItemValueSource::Settings) - DesiredValueRange.X);
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->SetValue(GetValue(EVSSettingItemValueSource::Settings));
		}
	}

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(GetValueContentText(Value));
		}
	}
}

void UVSSettingItem_ConsoleVariable_Integer::SetValue_Implementation(int32 InValue)
{
	if (Value != InValue) return;
	Value = InValue;
	NotifyValueUpdate();
}

int32 UVSSettingItem_ConsoleVariable_Integer::GetValue_Implementation(EVSSettingItemValueSource::Type ValueType) const
{
	switch (ValueType)
	{
	case EVSSettingItemValueSource::Default:
		return GetConsoleVariable() ? FCString::Atoi(*GetConsoleVariable()->GetDefaultValue()) : false;
			
	case EVSSettingItemValueSource::Current:
		return GetConsoleVariable() ? GetConsoleVariable()->GetInt() : false;
			
	case EVSSettingItemValueSource::Settings:
		return Value;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedValue;
		
	default:
		return 0;
	}
}

void UVSSettingItem_ConsoleVariable_Integer::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	if (Value != Variable->GetInt())
	{
		if (bListenAndOverride)
		{
			Variable->Set(Value);
		}
		else
		{
			SetValue(Variable->GetInt());
		}
	}
}
