// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_Float.h"

UVSSettingItem_ConsoleVariable_Float::UVSSettingItem_ConsoleVariable_Float(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer)
{
	
}
void UVSSettingItem_ConsoleVariable_Float::Load_Implementation()
{
	GConfig->GetFloat(TEXT("ConsoleVariable"), *(TEXT("Float.") + ConsoleVariableName), Value, GGameUserSettingsIni);
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

void UVSSettingItem_ConsoleVariable_Float::SetValue(float InValue)
{
	if (FMath::IsNearlyEqual(Value, InValue)) return;
	Value = InValue;
	NotifyUpdate();
}

float UVSSettingItem_ConsoleVariable_Float::GetValue(EVSSettingItemValueSource::Type ValueType) const
{
	switch (ValueType)
	{
	case EVSSettingItemValueSource::Default:
		return GetConsoleVariable() ? FCString::Atof(*GetConsoleVariable()->GetDefaultValue()) : 0.f;
			
	case EVSSettingItemValueSource::Current:
		return GetConsoleVariable() ? GetConsoleVariable()->GetFloat() : 0.f;
			
	case EVSSettingItemValueSource::Settings:
		return Value;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedValue;
		
	default:
		return 0.f;
	}
}
