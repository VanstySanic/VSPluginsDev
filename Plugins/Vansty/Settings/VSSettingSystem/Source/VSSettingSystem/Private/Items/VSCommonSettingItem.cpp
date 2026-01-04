// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSCommonSettingItem.h"
#include "VSSettingSubsystem.h"

UVSCommonSettingItem::UVSCommonSettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::None);
}

void UVSCommonSettingItem::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	SetValueType(ValueType);
	LastEditorPreviewValue = EditorPreviewValue;
	SetEditorPreviewValueString(GetStringValue());
	LastEditorConfigParams = ConfigParams;
#endif
}

#if WITH_EDITOR
void UVSCommonSettingItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	bool bDesireReConfig = false;
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSCommonSettingItem, ValueType))
	{
		SetValueType(ValueType);
		
		if (ValueType == EVSCommonSettingValueType::None || ValueType == EVSCommonSettingValueType::String)
		{
			SetStringValue(FString());
		}
		
		bDesireReConfig = true;
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSCommonSettingItem, ConfigParams))
	{
		bDesireReConfig = true;
	}
	
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSCommonSettingItem, ValueType)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSCommonSettingItem, EditorPreviewValue))
	{
		if (ValueType == EVSCommonSettingValueType::None || ValueType == EVSCommonSettingValueType::String)
		{
			SetStringValue(EditorPreviewValue);
		}
		else
		{
			float EvalValue;
			if (FMath::Eval(EditorPreviewValue, EvalValue))
			{
				SetFloatValue(EvalValue);
			}
			else if (EditorPreviewValue == "true" || EditorPreviewValue == "false" || EditorPreviewValue.IsNumeric())
			{
				SetStringValue(EditorPreviewValue);
			}
			/** Roll back to last. */
			else
			{
				SetStringValue(LastEditorPreviewValue);
			}
		}
		
		SetEditorPreviewValueString(GetStringValue(EVSSettingItemValueSource::System));
	}

	if (bDesireReConfig && LastEditorConfigParams.bAutoDefaultConfig && ConfigParams.bAutoDefaultConfig)
	{
		/** Clear previous config and save new one. */
		FString ConfigString;
		if (GConfig->GetString(*LastEditorConfigParams.ConfigSection, *LastEditorConfigParams.ConfigKeyName, ConfigString, LastEditorConfigParams.ConfigFileName))
		{
			GConfig->RemoveKey(*LastEditorConfigParams.ConfigSection, *LastEditorConfigParams.ConfigKeyName, LastEditorConfigParams.ConfigFileName);
			
			ConfigString = GetUnionValueString(CurrentValue);
			GConfig->SetString(*ConfigParams.ConfigSection, *ConfigParams.ConfigKeyName, *ConfigString, ConfigParams.ConfigFileName);
		}

		LastEditorConfigParams = ConfigParams;
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSCommonSettingItem::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	SetValueType(ValueType);
}

void UVSCommonSettingItem::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();
	
#if WITH_EDITOR
	SetEditorPreviewValueString(GetStringValue(EVSSettingItemValueSource::System));

	if (!GIsPlayInEditorWorld && HasBeenInitialized())
	{
		ExecuteAction(EVSSettingItemAction::Apply);
		ExecuteAction(EVSSettingItemAction::Confirm);
	}
#endif
}

void UVSCommonSettingItem::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		SetBooleanValue(GetBooleanValue(ValueSource));
		break;
		
	case EVSCommonSettingValueType::Integer:
		SetIntegerValue(GetIntegerValue(ValueSource));
		break;
		
	case EVSCommonSettingValueType::LongInteger:
		SetLongIntegerValue(GetLongIntegerValue(ValueSource));
		break;
		
	case EVSCommonSettingValueType::Float:
		SetFloatValue(GetFloatValue(ValueSource));
		break;
		
	case EVSCommonSettingValueType::Double:
		SetDoubleValue(GetDoubleValue(ValueSource));
		break;
		
	case EVSCommonSettingValueType::None:
	case EVSCommonSettingValueType::String:
		SetStringValue(GetStringValue(ValueSource));
		
		break;
	default: ;
	}
}

bool UVSCommonSettingItem::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		return GetBooleanValue(EVSSettingItemValueSource::System) == GetBooleanValue(ValueSource);
		
	case EVSCommonSettingValueType::Integer:
		return GetIntegerValue(EVSSettingItemValueSource::System) == GetIntegerValue(ValueSource);
		
	case EVSCommonSettingValueType::LongInteger:
		return GetLongIntegerValue(EVSSettingItemValueSource::System) == GetLongIntegerValue(ValueSource);
	
	case EVSCommonSettingValueType::Float:
		return FMath::IsNearlyEqual(GetFloatValue(EVSSettingItemValueSource::System), GetFloatValue(ValueSource));
		
	case EVSCommonSettingValueType::Double:
		return FMath::IsNearlyEqual(GetDoubleValue(EVSSettingItemValueSource::System), GetDoubleValue(ValueSource));
		
	case EVSCommonSettingValueType::None:
	case EVSCommonSettingValueType::String:
		return GetStringValue(EVSSettingItemValueSource::System) == GetStringValue(ValueSource);

	default: ;
	}

	return false;
}

void UVSCommonSettingItem::Load_Implementation()
{
	Super::Load_Implementation();

	if (ConfigParams.bAutoDefaultConfig)
	{
		FString ConfigString;
		if (GConfig->GetString(*ConfigParams.ConfigSection, *ConfigParams.ConfigKeyName, ConfigString, ConfigParams.ConfigFileName))
		{
			SetUnionValueFromString(ConfigString, CurrentValue);
		}
	}
}

void UVSCommonSettingItem::Save_Implementation()
{
	Super::Save_Implementation();

	if (ConfigParams.bAutoDefaultConfig)
	{
		const FString& ConfigString = GetUnionValueString(CurrentValue);
		GConfig->SetString(*ConfigParams.ConfigSection, *ConfigParams.ConfigKeyName, *ConfigString, ConfigParams.ConfigFileName);
	}
}

void UVSCommonSettingItem::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	ConfirmedValue = CurrentValue;
}

bool UVSCommonSettingItem::GetBooleanValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (ValueType == CurrentValue.GetCurrentSubtypeIndex() - 1)
	{
		return GetValueFromString<bool>(GetStringValue(ValueSource));
	}
	
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		switch (ValueSource)
		{
		/** Implement the two cases in child classes. */
		case EVSSettingItemValueSource::Default:
		case EVSSettingItemValueSource::Game:
			return false;
			
		case EVSSettingItemValueSource::System:
			return CurrentValue.GetSubtype<bool>();
			
		case EVSSettingItemValueSource::Confirmed:
			return ConfirmedValue.GetSubtype<bool>();
			
		default: ;
		}
		return false;
		
	case EVSCommonSettingValueType::Integer:
		return static_cast<bool>(GetIntegerValue(ValueSource));

	case EVSCommonSettingValueType::LongInteger:
		return static_cast<bool>(GetLongIntegerValue(ValueSource));
		
	case EVSCommonSettingValueType::Float:
		return static_cast<bool>(GetFloatValue(ValueSource));
		
	case EVSCommonSettingValueType::Double:
		return static_cast<bool>(GetDoubleValue(ValueSource));
		
	default: ;
	}
	
	return false;
}

int32 UVSCommonSettingItem::GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (ValueType == CurrentValue.GetCurrentSubtypeIndex() - 1)
	{
		return GetValueFromString<int32>(GetStringValue(ValueSource));
	}
	
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		return static_cast<int32>(GetBooleanValue());
		
	case EVSCommonSettingValueType::Integer:
		switch (ValueSource)
		{
		/** Implement the two cases in child classes. */
		case EVSSettingItemValueSource::Default:
		case EVSSettingItemValueSource::Game:
			return 0;
			
		case EVSSettingItemValueSource::System:
			return CurrentValue.GetSubtype<int32>();
			
		case EVSSettingItemValueSource::Confirmed:
			return ConfirmedValue.GetSubtype<int32>();
			
		default: ;
		}
		return 0;

	case EVSCommonSettingValueType::LongInteger:
		return static_cast<int32>(GetLongIntegerValue(ValueSource));
		
	case EVSCommonSettingValueType::Float:
		return static_cast<int32>(GetFloatValue(ValueSource));
		
	case EVSCommonSettingValueType::Double:
		return static_cast<int32>(GetDoubleValue(ValueSource));
		
	default: ;
	}
	
	return 0;
}

int64 UVSCommonSettingItem::GetLongIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (ValueType == CurrentValue.GetCurrentSubtypeIndex() - 1)
	{
		return GetValueFromString<int64>(GetStringValue(ValueSource));
	}
	
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		return static_cast<int64>(GetBooleanValue());
		
	case EVSCommonSettingValueType::Integer:
		return static_cast<int64>(GetIntegerValue(ValueSource));

	case EVSCommonSettingValueType::LongInteger:
		switch (ValueSource)
		{
		/** Implement the two cases in child classes. */
		case EVSSettingItemValueSource::Default:
		case EVSSettingItemValueSource::Game:
			return 0;
			
		case EVSSettingItemValueSource::System:
			return CurrentValue.GetSubtype<int64>();
			
		case EVSSettingItemValueSource::Confirmed:
			return ConfirmedValue.GetSubtype<int64>();
			
		default: ;
		}
		return 0;
		
	case EVSCommonSettingValueType::Float:
		return static_cast<int64>(GetFloatValue(ValueSource));
		
	case EVSCommonSettingValueType::Double:
		return static_cast<int64>(GetDoubleValue(ValueSource));
		
	default: ;
	}
	
	return 0;
}

float UVSCommonSettingItem::GetFloatValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (ValueType == CurrentValue.GetCurrentSubtypeIndex() - 1)
	{
		return GetValueFromString<float>(GetStringValue(ValueSource));
	}
	
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		return static_cast<float>(GetBooleanValue());
		
	case EVSCommonSettingValueType::Integer:
		return static_cast<float>(GetIntegerValue(ValueSource));

	case EVSCommonSettingValueType::LongInteger:
		return static_cast<float>(GetLongIntegerValue(ValueSource));
		
	case EVSCommonSettingValueType::Float:
		switch (ValueSource)
		{
		/** Implement the two cases in child classes. */
		case EVSSettingItemValueSource::Default:
		case EVSSettingItemValueSource::Game:
			return 0.f;
			
		case EVSSettingItemValueSource::System:
			return CurrentValue.GetSubtype<float>();
			
		case EVSSettingItemValueSource::Confirmed:
			return ConfirmedValue.GetSubtype<float>();
			
		default: ;
		}
		return 0.f;
		
	case EVSCommonSettingValueType::Double:
		return static_cast<float>(GetDoubleValue(ValueSource));
	
	default: ;
	}
	
	return 0.f;
}

double UVSCommonSettingItem::GetDoubleValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (ValueType == CurrentValue.GetCurrentSubtypeIndex() - 1)
	{
		return GetValueFromString<double>(GetStringValue(ValueSource));
	}
	
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		return static_cast<double>(GetBooleanValue());
		
	case EVSCommonSettingValueType::Integer:
		return static_cast<double>(GetIntegerValue(ValueSource));

	case EVSCommonSettingValueType::LongInteger:
		return static_cast<double>(GetLongIntegerValue(ValueSource));
		
	case EVSCommonSettingValueType::Float:
		return static_cast<double>(GetFloatValue(ValueSource));
		
	case EVSCommonSettingValueType::Double:
		switch (ValueSource)
		{
		/** Implement the two cases in child classes. */
		case EVSSettingItemValueSource::Default:
		case EVSSettingItemValueSource::Game:
			return 0.0;
	
		case EVSSettingItemValueSource::System:
			return CurrentValue.GetSubtype<double>();
			
		case EVSSettingItemValueSource::Confirmed:
			return ConfirmedValue.GetSubtype<double>();
			
		default: ;
		}
		return 0.0;

	default: ;
	}
	
	return 0.0;
}

FString UVSCommonSettingItem::GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	static FString EmptyString = FString();
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		return GetStringFromValue<bool>(GetBooleanValue(ValueSource));
		
	case EVSCommonSettingValueType::Integer:
		return GetStringFromValue<int32>(GetIntegerValue(ValueSource));

	case EVSCommonSettingValueType::LongInteger:
		return GetStringFromValue<int64>(GetLongIntegerValue(ValueSource));
		
	case EVSCommonSettingValueType::Float:
		return GetStringFromValue<float>(GetFloatValue(ValueSource));
		
	case EVSCommonSettingValueType::Double:
		return GetStringFromValue<double>(GetDoubleValue(ValueSource));

	case EVSCommonSettingValueType::None:
	case EVSCommonSettingValueType::String:
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			/** Implement the case in child classes. */
			return EmptyString;
			
		case EVSSettingItemValueSource::Game:
			/** Implement the case in child classes. */
			return EmptyString;
			
		case EVSSettingItemValueSource::System:
			return CurrentValue.GetSubtype<FString>();
			
		case EVSSettingItemValueSource::Confirmed:
			return ConfirmedValue.GetSubtype<FString>();
			
		default: ;
		}
		return EmptyString;
		
	default: ;
	}

	return EmptyString;
}

void UVSCommonSettingItem::SetBooleanValue(bool bNewValue)
{
	const TValueUnion PrevValue = CurrentValue;
	SetUnionValue<bool>(bNewValue, CurrentValue);
	if (!IsValueValid())
	{
		Validate();
	}
	else if (CurrentValue != PrevValue)
	{
		NotifyValueUpdate(true);
	}
}

void UVSCommonSettingItem::SetIntegerValue(int32 NewValue)
{
	const TValueUnion PrevValue = CurrentValue;
	SetUnionValue<int32>(NewValue, CurrentValue);
	if (!IsValueValid())
	{
		Validate();
	}
	else if (CurrentValue != PrevValue)
	{
		NotifyValueUpdate(true);
	}
}

void UVSCommonSettingItem::SetLongIntegerValue(int64 NewValue)
{
	const TValueUnion PrevValue = CurrentValue;
	SetUnionValue<int64>(NewValue, CurrentValue);
	if (!IsValueValid())
	{
		Validate();
	}
	else if (CurrentValue != PrevValue)
	{
		NotifyValueUpdate(true);
	}
}

void UVSCommonSettingItem::SetFloatValue(float NewValue)
{
	const TValueUnion PrevValue = CurrentValue;
	SetUnionValue<float>(NewValue, CurrentValue);
	if (!IsValueValid())
	{
		Validate();
	}
	else if (CurrentValue != PrevValue)
	{
		NotifyValueUpdate(true);
	}
}

void UVSCommonSettingItem::SetDoubleValue(double NewValue)
{
	const TValueUnion PrevValue = CurrentValue;
	SetUnionValue<double>(NewValue, CurrentValue);
	if (!IsValueValid())
	{
		Validate();
	}
	else if (CurrentValue != PrevValue)
	{
		NotifyValueUpdate(true);
	}
}

void UVSCommonSettingItem::SetStringValue(const FString& NewValue)
{
	const TValueUnion PrevValue = CurrentValue;
	SetUnionValue<FString>(NewValue, CurrentValue);
	if (!IsValueValid())
	{
		Validate();
	}
	else if (CurrentValue != PrevValue)
	{
		NotifyValueUpdate(true);
	}
}

void UVSCommonSettingItem::SetValueType(EVSCommonSettingValueType::Type NewValueType)
{
	if (ValueType == NewValueType && (ValueType == CurrentValue.GetCurrentSubtypeIndex() + 1)) return;
	ValueType = NewValueType;

	FString ValueString = GetUnionValueString(CurrentValue);

	/** Set union value type. */
	switch (ValueType)
	{
	case EVSCommonSettingValueType::Boolean:
		CurrentValue.SetSubtype<bool>();
		ConfirmedValue.SetSubtype<bool>();
		break;
		
	case EVSCommonSettingValueType::Integer:
		CurrentValue.SetSubtype<int32>();
		ConfirmedValue.SetSubtype<int32>();
		break;
		
	case EVSCommonSettingValueType::LongInteger:
		CurrentValue.SetSubtype<int64>();
		ConfirmedValue.SetSubtype<int64>();
		break;
		
	case EVSCommonSettingValueType::Float:
		CurrentValue.SetSubtype<float>();
		ConfirmedValue.SetSubtype<float>();
		break;
		
	case EVSCommonSettingValueType::Double:
		CurrentValue.SetSubtype<double>();
		ConfirmedValue.SetSubtype<double>();
		break;
		
	case EVSCommonSettingValueType::None:
	case EVSCommonSettingValueType::String:
		CurrentValue.SetSubtype<FString>();
		ConfirmedValue.SetSubtype<FString>();
		break;
		
	default: ;
	}

	SetUnionValueFromString(ValueString, CurrentValue);

#if WITH_EDITOR
	SetEditorPreviewValueString(GetStringValue());
#endif
}

#if WITH_EDITOR
bool UVSCommonSettingItem::AllowEditorChangingEditorPreviewValue_Implementation() const
{
	return true;
}

void UVSCommonSettingItem::SetEditorPreviewValueString(const FString& NewValue)
{
	EditorPreviewValue = NewValue;
	
	int32 Index = INDEX_NONE;
	if (ValueType != EVSCommonSettingValueType::None && ValueType != EVSCommonSettingValueType::String)
	{
		if (EditorPreviewValue.Contains("."))
		{
			int32 DotIndex;
			EditorPreviewValue.FindChar('.', DotIndex);
		
			while (EditorPreviewValue.FindLastChar('0', Index))
			{
				if (Index < DotIndex || Index != EditorPreviewValue.Len() - 1) break;
				EditorPreviewValue.RemoveFromEnd("0");
			}
			if (DotIndex == EditorPreviewValue.Len() - 1)
			{
				EditorPreviewValue.RemoveFromEnd(".");
				if (EditorPreviewValue.IsEmpty())
				{
					EditorPreviewValue = FString("0");
				}
			}
		}
	}
	LastEditorPreviewValue = EditorPreviewValue;
}
#endif

#if WITH_EDITOR
bool UVSCommonSettingItem::AllowEditorChangingValueType_Implementation() const
{
	return true;
}

bool UVSCommonSettingItem::AllowEditorChangingConfigParams_Implementation() const
{
	return true;
}
#endif

FString UVSCommonSettingItem::GetUnionValueString(const TValueUnion& UnionValue) const
{
	static FString EmptyString = FString();

	switch (static_cast<EVSCommonSettingValueType::Type>(UnionValue.GetCurrentSubtypeIndex() + 1))
	{
	case EVSCommonSettingValueType::Boolean:
		return GetStringFromValue<bool>(UnionValue.GetSubtype<bool>());
		
	case EVSCommonSettingValueType::Integer:
		return GetStringFromValue<int32>(UnionValue.GetSubtype<int32>());

	case EVSCommonSettingValueType::LongInteger:
		return GetStringFromValue<int64>(UnionValue.GetSubtype<int64>());

	case EVSCommonSettingValueType::Float:
		return GetStringFromValue<float>(UnionValue.GetSubtype<float>());

	case EVSCommonSettingValueType::Double:
		return GetStringFromValue<double>(UnionValue.GetSubtype<double>());
	
	case EVSCommonSettingValueType::None:
	case EVSCommonSettingValueType::String:
		return GetStringFromValue<FString>(UnionValue.GetSubtype<FString>());
		
	default: ;
	}

	return EmptyString;
}

void UVSCommonSettingItem::SetUnionValueFromString(const FString& String, TValueUnion& UnionValue) const
{
	switch (static_cast<EVSCommonSettingValueType::Type>(UnionValue.GetCurrentSubtypeIndex() + 1))
	{
	case EVSCommonSettingValueType::Boolean:
		UnionValue.SetSubtype<bool>(GetValueFromString<bool>(String));
		break;
		
	case EVSCommonSettingValueType::Integer:
		UnionValue.SetSubtype<int32>(GetValueFromString<int32>(String));
		break;
		
	case EVSCommonSettingValueType::LongInteger:
		UnionValue.SetSubtype<int64>(GetValueFromString<int64>(String));
		break;
		
	case EVSCommonSettingValueType::Float:
		UnionValue.SetSubtype<float>(GetValueFromString<float>(String));
		break;
		
	case EVSCommonSettingValueType::Double:
		UnionValue.SetSubtype<double>(GetValueFromString<double>(String));
		break;
		
	case EVSCommonSettingValueType::None:
	case EVSCommonSettingValueType::String:
		UnionValue.SetSubtype<FString>(String);
		break;
		
	default: ;
	}
}
