// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_Float.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

UVSSettingItem_ConsoleVariable_Float::UVSSettingItem_ConsoleVariable_Float(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ConsoleVariable_Float::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
	check(ConsoleVariable);

	OnVariableChangedDelegateHandle = ConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSSettingItem_ConsoleVariable_Float::OnConsoleVariableChanged);
}

void UVSSettingItem_ConsoleVariable_Float::Uninitialize_Implementation()
{
	if (OnVariableChangedDelegateHandle.IsValid())
	{
		ConsoleVariable->OnChangedDelegate().Remove(OnVariableChangedDelegateHandle);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSSettingItem_ConsoleVariable_Float::Load_Implementation()
{
	float LoadedValue = GetValue(EVSSettingItemValueSource::Settings);
	GConfig->GetFloat(TEXT("ConsoleVariable"), *(TEXT("Float.") + ConsoleVariableName), LoadedValue, GGameUserSettingsIni);
	SetValue(LoadedValue);
}

void UVSSettingItem_ConsoleVariable_Float::Apply_Implementation()
{
	GetConsoleVariable()->Set(Value);
}

void UVSSettingItem_ConsoleVariable_Float::Confirm_Implementation()
{
	LastConfirmedValue = Value;
}

void UVSSettingItem_ConsoleVariable_Float::Save_Implementation()
{
	GConfig->SetFloat(TEXT("ConsoleVariable"), *(TEXT("Float.") + ConsoleVariableName), Value, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Float::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItem_ConsoleVariable_Float::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return Value == GetValue(ValueSource);
}

void UVSSettingItem_ConsoleVariable_Float::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
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

void UVSSettingItem_ConsoleVariable_Float::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	if (!FMath::IsNearlyEqual(Value, Variable->GetFloat()))
	{
		if (bListenAndOverride)
		{
			Variable->Set(Value);
		}
		else
		{
			SetValue(Variable->GetFloat());
		}
	}
}

void UVSSettingItem_ConsoleVariable_Float::SetValue_Implementation(float InValue)
{
	if (Value == InValue) return;
	Value = InValue;
	NotifyValueUpdate();
}

float UVSSettingItem_ConsoleVariable_Float::GetValue_Implementation(EVSSettingItemValueSource::Type ValueType) const
{
	switch (ValueType)
	{
	case EVSSettingItemValueSource::Default:
		return GetConsoleVariable() ? FCString::Atof(*GetConsoleVariable()->GetDefaultValue()) : false;
			
	case EVSSettingItemValueSource::Current:
		return GetConsoleVariable() ? GetConsoleVariable()->GetFloat() : false;
			
	case EVSSettingItemValueSource::Settings:
		return Value;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedValue;
		
	default:
		return 0.f;
	}
}