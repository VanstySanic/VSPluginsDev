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
		if (CurrentConsoleVariable && OnVariableChangedDelegateHandle.IsValid())
		{
			CurrentConsoleVariable->OnChangedDelegate().Remove(OnVariableChangedDelegateHandle);
		}
		
		CurrentConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName, false);
		if (CurrentConsoleVariable)
		{
			if (CurrentConsoleVariable && !OnVariableChangedDelegateHandle.IsValid())
			{
				OnVariableChangedDelegateHandle = CurrentConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSConsoleVariableSettingItem::OnConsoleVariableChanged);
			}
			
			if (CurrentConsoleVariable->IsVariableBool())
			{
				SetValueType(EVSCommonSettingValueType::Boolean);
			}
			else if (CurrentConsoleVariable->IsVariableInt())
			{
				SetValueType(EVSCommonSettingValueType::Integer);
			}
			else if (CurrentConsoleVariable->GetFloat())
			{
				SetValueType(EVSCommonSettingValueType::Float);
			}
			else if (CurrentConsoleVariable->GetFloat())
			{
				SetValueType(EVSCommonSettingValueType::String);
			}
		}
		else
		{
			SetValueType(EVSCommonSettingValueType::None);
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSConsoleVariableSettingItem::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (CurrentConsoleVariable && !OnVariableChangedDelegateHandle.IsValid())
	{
		OnVariableChangedDelegateHandle = CurrentConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSConsoleVariableSettingItem::OnConsoleVariableChanged);
	}
}

void UVSConsoleVariableSettingItem::Uninitialize_Implementation()
{
	if (CurrentConsoleVariable && OnVariableChangedDelegateHandle.IsValid())
	{
		CurrentConsoleVariable->OnChangedDelegate().Remove(OnVariableChangedDelegateHandle);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSConsoleVariableSettingItem::Apply_Implementation()
{
	Super::Apply_Implementation();

	if (CurrentConsoleVariable)
	{
		CachedAppliedValueString = CurrentConsoleVariable->GetString();
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
	
	if (ValueType == EVSCommonSettingValueType::Integer)
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
	
	if (ValueType == EVSCommonSettingValueType::Integer)
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

void UVSConsoleVariableSettingItem::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	switch (ConsoleVariableChangePolicy) {
	case EVSSettingItemConsoleVariableChangePolicy::Override:
		{
			const FString& SystemValueString = GetStringValue(EVSSettingItemValueSource::System);
			if (CachedAppliedValueString != Variable->GetString())
			{
				Variable->SetWithCurrentPriority(*SystemValueString);
				CachedAppliedValueString = SystemValueString;
			}
			break;
		}

	case EVSSettingItemConsoleVariableChangePolicy::Absorb:
		SetStringValue(Variable->GetString());
		ExecuteActions(TArray<TEnumAsByte<EVSSettingItemAction::Type>>
		{
			EVSSettingItemAction::Apply,
#if WITH_EDITOR
			EVSSettingItemAction::Confirm,
#endif
		});
		break;
		
	default: ;
	}
}

#if WITH_EDITOR
bool UVSConsoleVariableSettingItem::AllowChangingValueType_Implementation() const
{
	return false;
}
#endif
