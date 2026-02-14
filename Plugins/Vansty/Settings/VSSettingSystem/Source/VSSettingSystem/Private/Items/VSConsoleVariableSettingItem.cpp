// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSConsoleVariableSettingItem.h"

UVSConsoleVariableSettingItem::UVSConsoleVariableSettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

#if WITH_EDITOR
void UVSConsoleVariableSettingItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSConsoleVariableSettingItem, ConsoleVariableName))
	{
		SetConsoleVariableName(ConsoleVariableName);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSConsoleVariableSettingItem::PostLoad()
{
	Super::PostLoad();

	SetConsoleVariableName(ConsoleVariableName);
}

void UVSConsoleVariableSettingItem::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	CurrentConsoleVariable = CurrentConsoleVariable ? CurrentConsoleVariable : IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName, false);
	if (CurrentConsoleVariable && !CurrentConsoleVariable->OnChangedDelegate().IsBoundToObject(this))
	{
		CurrentConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSConsoleVariableSettingItem::OnConsoleVariableChanged);
		SetStringValue(CurrentConsoleVariable->GetString());
	}
}

void UVSConsoleVariableSettingItem::Uninitialize_Implementation()
{
	if (CurrentConsoleVariable && CurrentConsoleVariable->OnChangedDelegate().IsBoundToObject(this))
	{
		CurrentConsoleVariable->OnChangedDelegate().RemoveAll(this);
	}

	CurrentConsoleVariable = nullptr;
	
	Super::Uninitialize_Implementation();
}

void UVSConsoleVariableSettingItem::Apply_Implementation()
{
	Super::Apply_Implementation();

	if (CurrentConsoleVariable)
	{
		CurrentConsoleVariable->OnChangedDelegate().RemoveAll(this);
		
		CurrentConsoleVariable->SetWithCurrentPriority(*GetStringValue(EVSSettingItemValueSource::System));
		
		CurrentConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSConsoleVariableSettingItem::OnConsoleVariableChanged);
	}
}

bool UVSConsoleVariableSettingItem::GetBooleanValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!CurrentConsoleVariable) return Super::GetBooleanValue_Implementation(ValueSource);
	
	if (ValueType == EVSCommonSettingValueType::Boolean)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return FCString::ToBool(*CurrentConsoleVariable->GetDefaultValue());
			
		case EVSSettingItemValueSource::Game:
			return CurrentConsoleVariable->GetBool();
			
		default: ;
		}
	}
		
	return Super::GetBooleanValue_Implementation(ValueSource);
}

int32 UVSConsoleVariableSettingItem::GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!CurrentConsoleVariable) return Super::GetIntegerValue_Implementation(ValueSource);
	
	if (ValueType == EVSCommonSettingValueType::Integer)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return FCString::Atoi(*CurrentConsoleVariable->GetDefaultValue());
			
		case EVSSettingItemValueSource::Game:
			return CurrentConsoleVariable->GetInt();
			
		default: ;
		}
	}
		
	return Super::GetIntegerValue_Implementation(ValueSource);
}

int64 UVSConsoleVariableSettingItem::GetLongIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<int64>(GetIntegerValue(ValueSource));
}

float UVSConsoleVariableSettingItem::GetFloatValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!CurrentConsoleVariable) return Super::GetFloatValue_Implementation(ValueSource);
	
	if (ValueType == EVSCommonSettingValueType::Float)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return FCString::Atof(*CurrentConsoleVariable->GetDefaultValue());
			
		case EVSSettingItemValueSource::Game:
			return CurrentConsoleVariable->GetFloat();
			
		default: ;
		}
	}
		
	return Super::GetFloatValue_Implementation(ValueSource);
}

double UVSConsoleVariableSettingItem::GetDoubleValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<double>(GetFloatValue(ValueSource));
}

FString UVSConsoleVariableSettingItem::GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!CurrentConsoleVariable) return Super::GetStringValue_Implementation(ValueSource);
	
	if (ValueType == EVSCommonSettingValueType::String)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return CurrentConsoleVariable->GetDefaultValue();
			
		case EVSSettingItemValueSource::Game:
			return CurrentConsoleVariable->GetString();
			
		default: ;
		}
	}
		
	return Super::GetStringValue_Implementation(ValueSource);
}

#if WITH_EDITOR
bool UVSConsoleVariableSettingItem::EditorAllowChangingConsoleVariableName_Implementation() const
{
	return true;
}
#endif

void UVSConsoleVariableSettingItem::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	Variable->OnChangedDelegate().RemoveAll(this);
	NotifyValueExternChanged(false);
	Variable->OnChangedDelegate().AddUObject(this, &UVSConsoleVariableSettingItem::OnConsoleVariableChanged);
}

#if WITH_EDITOR
bool UVSConsoleVariableSettingItem::EditorAllowChangingValueType_Implementation() const
{
	return false;
}
#endif

void UVSConsoleVariableSettingItem::SetConsoleVariableName(FString VariableName)
{
	if (ConsoleVariableName == VariableName && CurrentConsoleVariable) return;
	ConsoleVariableName = VariableName;
	
	if (CurrentConsoleVariable)
	{
		CurrentConsoleVariable->OnChangedDelegate().RemoveAll(this);
	}
	
	CurrentConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*VariableName, false);
	if (CurrentConsoleVariable)
	{
		if (CurrentConsoleVariable->IsVariableBool())
		{
			SetValueType(EVSCommonSettingValueType::Boolean);
		}
		else if (CurrentConsoleVariable->IsVariableInt())
		{
			SetValueType(EVSCommonSettingValueType::Integer);
		}
		else if (CurrentConsoleVariable->IsVariableFloat())
		{
			SetValueType(EVSCommonSettingValueType::Float);
		}
		else if (CurrentConsoleVariable->IsVariableString())
		{
			SetValueType(EVSCommonSettingValueType::String);
		}

		if (HasBeenInitialized())
		{
			CurrentConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSConsoleVariableSettingItem::OnConsoleVariableChanged);
			SetStringValue(CurrentConsoleVariable->GetString());
		}
	}
	else
	{
		SetValueType(EVSCommonSettingValueType::None);

		if (HasBeenInitialized())
		{
			SetStringValue(FString());
		}
	}
}
