// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariable_Boolean.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"

UVSSettingItem_ConsoleVariable_Boolean::UVSSettingItem_ConsoleVariable_Boolean(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UVSSettingItem_ConsoleVariable_Boolean::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
	check(ConsoleVariable);

	OnVariableChangedDelegateHandle = ConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSSettingItem_ConsoleVariable_Boolean::OnConsoleVariableChanged);
}

void UVSSettingItem_ConsoleVariable_Boolean::Uninitialize_Implementation()
{
	if (OnVariableChangedDelegateHandle.IsValid())
	{
		ConsoleVariable->OnChangedDelegate().Remove(OnVariableChangedDelegateHandle);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSSettingItem_ConsoleVariable_Boolean::Load_Implementation()
{
	bool bLoadedValue = GetValue(EVSSettingItemValueSource::Settings);
	GConfig->GetBool(TEXT("ConsoleVariable"), *(TEXT("Bool.") + ConsoleVariableName), bLoadedValue, GGameUserSettingsIni);
	SetValue(bLoadedValue);
}

void UVSSettingItem_ConsoleVariable_Boolean::Apply_Implementation()
{
	GetConsoleVariable()->Set(bValue);
}

void UVSSettingItem_ConsoleVariable_Boolean::Confirm_Implementation()
{
	bLastConfirmedValue = bValue;
}

void UVSSettingItem_ConsoleVariable_Boolean::Save_Implementation()
{
	GConfig->SetBool(TEXT("ConsoleVariable"), *(TEXT("Bool.") + ConsoleVariableName), bValue, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ConsoleVariable_Boolean::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetValue(GetValue(ValueSource));
}

bool UVSSettingItem_ConsoleVariable_Boolean::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return bValue == GetValue(ValueSource);
}

void UVSSettingItem_ConsoleVariable_Boolean::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			ComboBox->SetSelectedOption(NamedValues.FindRef(GetValue(EVSSettingItemValueSource::Settings)).ToString());
		}
		else if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			CheckBox->SetIsChecked(GetValue(EVSSettingItemValueSource::Settings));
		}
	}

	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(NamedValues.FindRef(GetValue(EVSSettingItemValueSource::Settings)));
		}
	}
}

void UVSSettingItem_ConsoleVariable_Boolean::OnConsoleVariableChanged(IConsoleVariable* Variable)
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

void UVSSettingItem_ConsoleVariable_Boolean::SetValue_Implementation(bool bInValue)
{
	if (bValue == bInValue) return;
	bValue = bInValue;
	NotifyValueUpdate();
}

bool UVSSettingItem_ConsoleVariable_Boolean::GetValue_Implementation(EVSSettingItemValueSource::Type ValueType) const
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