// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_Int.h"

UVSSettingItem_ConsoleVariable_Int::UVSSettingItem_ConsoleVariable_Int(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ConsoleVariable_Int::Load_Implementation()
{
	GConfig->GetInt(TEXT("ConsoleVariable"), *(TEXT("Bool.") + ConsoleVariableName), Value, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Int::Apply_Implementation()
{
	GetConsoleVariable()->Set(Value);
}

void UVSSettingItem_ConsoleVariable_Int::Confirm_Implementation()
{
	LastConfirmedValue = Value;
}

void UVSSettingItem_ConsoleVariable_Int::Save_Implementation()
{
	GConfig->SetInt(TEXT("ConsoleVariable"), *(TEXT("Int.") + ConsoleVariableName), Value, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Int::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItem_ConsoleVariable_Int::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return Value == GetValue(ValueSource);
}

void UVSSettingItem_ConsoleVariable_Int::OnConsoleVariableChanged(IConsoleVariable* Variable)
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
	}}

void UVSSettingItem_ConsoleVariable_Int::SetValue(int32 InValue)
{
	if (Value != InValue) return;
	Value = InValue;
	NotifyUpdate();
}

int32 UVSSettingItem_ConsoleVariable_Int::GetValue(EVSSettingItemValueSource::Type ValueType) const
{
	switch (ValueType)
	{
	case EVSSettingItemValueSource::Default:
		return GetConsoleVariable() ? FCString::Atoi(*GetConsoleVariable()->GetDefaultValue()) : 0;
			
	case EVSSettingItemValueSource::Current:
		return GetConsoleVariable() ? GetConsoleVariable()->GetInt() : 0;
			
	case EVSSettingItemValueSource::Settings:
		return Value;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedValue;
		
	default:
		return 0;
	}
}
