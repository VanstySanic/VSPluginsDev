// Copyright VanstySanic. All Rights Reserved.


#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_String.h"

UVSSettingItem_ConsoleVariable_String::UVSSettingItem_ConsoleVariable_String(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ConsoleVariable_String::Load_Implementation()
{
	GConfig->GetString(TEXT("ConsoleVariable"), *(TEXT("String.") + ConsoleVariableName), Value, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_String::Apply_Implementation()
{
	GetConsoleVariable()->Set(*Value);
}

void UVSSettingItem_ConsoleVariable_String::Confirm_Implementation()
{
	LastConfirmedValue = Value;
}

void UVSSettingItem_ConsoleVariable_String::Save_Implementation()
{
	GConfig->SetString(TEXT("ConsoleVariable"), *(TEXT("Float.") + ConsoleVariableName), *Value, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_String::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItem_ConsoleVariable_String::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return Value == GetValue(ValueSource);
}

void UVSSettingItem_ConsoleVariable_String::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	if (Value != Variable->GetString())
	{
		if (bListenAndOverride)
		{
			Variable->Set(*Value);
		}
		else
		{
			SetValue(Variable->GetString());
		}
	}
}

void UVSSettingItem_ConsoleVariable_String::SetValue(const FString& InValue)
{
	if (Value != InValue)
	Value = InValue;
	NotifyUpdate();
}

FString UVSSettingItem_ConsoleVariable_String::GetValue(EVSSettingItemValueSource::Type ValueType) const
{
	switch (ValueType)
	{
	case EVSSettingItemValueSource::Default:
		return GetConsoleVariable() ? FString(*GetConsoleVariable()->GetDefaultValue()) : FString();
			
	case EVSSettingItemValueSource::Current:
		return GetConsoleVariable() ? GetConsoleVariable()->GetString() : FString();
			
	case EVSSettingItemValueSource::Settings:
		return Value;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedValue;
		
	default:
		return FString();
	}
}
