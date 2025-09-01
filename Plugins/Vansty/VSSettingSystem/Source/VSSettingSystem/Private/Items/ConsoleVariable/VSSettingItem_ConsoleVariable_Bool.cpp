// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_Bool.h"

UVSSettingItem_ConsoleVariable_Bool::UVSSettingItem_ConsoleVariable_Bool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ConsoleVariable_Bool::Load_Implementation()
{
	GConfig->GetBool(TEXT("ConsoleVariable"), *(TEXT("Bool.") + ConsoleVariableName), bValue, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Bool::Apply_Implementation()
{
	GetConsoleVariable()->Set(bValue);
}

void UVSSettingItem_ConsoleVariable_Bool::Confirm_Implementation()
{
	bLastConfirmedValue = bValue;
}

void UVSSettingItem_ConsoleVariable_Bool::Save_Implementation()
{
	GConfig->SetBool(TEXT("ConsoleVariable"), *(TEXT("Bool.") + ConsoleVariableName), bValue, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Bool::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItem_ConsoleVariable_Bool::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return bValue == GetValue(ValueSource);
}

void UVSSettingItem_ConsoleVariable_Bool::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	if (bValue != Variable->GetBool())
	{
		if (bListenAndOverride)
		{
			Variable->Set(bValue);
		}
		else
		{
			SetValue(Variable->GetBool());
		}
	}
}

void UVSSettingItem_ConsoleVariable_Bool::SetValue(bool bInValue)
{
	if (bValue == bInValue) return;
	bValue = bInValue;
	NotifyUpdate();
}

bool UVSSettingItem_ConsoleVariable_Bool::GetValue(EVSSettingItemValueSource::Type ValueType) const
{
	switch (ValueType)
	{
	case EVSSettingItemValueSource::Default:
		return GetConsoleVariable() ? FCString::ToBool(*GetConsoleVariable()->GetDefaultValue()) : false;
			
	case EVSSettingItemValueSource::Current:
		return GetConsoleVariable() ? GetConsoleVariable()->GetBool() : false;
			
	case EVSSettingItemValueSource::Settings:
		return bValue;

	case EVSSettingItemValueSource::LastConfirmed:
		return bLastConfirmedValue;
		
	default:
		return false;
	}
}
