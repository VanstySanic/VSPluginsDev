// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "Containers/Union.h"
#include "VSCommonSettingItem.generated.h"

/** Supported value representation for a common setting item. */
UENUM(BlueprintType)
namespace EVSCommonSettingValueType
{
	enum Type
	{
		None	UMETA(Hidden),
		Boolean,
		Integer,
		LongInteger,
		Float,
		Double,
		String,
	};
}

/**
 * Base setting item that stores a single value in a small set of common types.
 *
 * Provides typed getters/setters for Blueprint and implements the setting item lifecycle
 * (load/save/confirm and source comparisons) using an internal union-backed storage.
 * Optionally binds to config to persist the system/confirmed values.
 */
UCLASS(Abstract, DisplayName = "VS.Settings.Item.Common")
class VSSETTINGSYSTEM_API UVSCommonSettingItem : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

	/** Internal value storage used for current/confirmed state. */
	using TValueUnion = TUnion<bool, int32, int64, float, double, FString>;
	
public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItem Interface
	virtual void Load_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnValueUpdated_Implementation() override;
	//~ End UVSSettingItem Interface

public:
	/** Returns the value as bool. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	bool GetBooleanValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Returns the value as int32. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	int32 GetIntegerValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Returns the value as int64. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	int64 GetLongIntegerValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
	
	/** Returns the value as float. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	float GetFloatValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Returns the value as double. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	double GetDoubleValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Returns the value as FString. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	FString GetStringValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Sets the current value as bool and triggers update notification. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetBooleanValue(bool bNewValue);

	/** Sets the current value as int32 and triggers update notification. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetIntegerValue(int32 NewValue);

	/** Sets the current value as int64 and triggers update notification. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetLongIntegerValue(int64 NewValue);
	
	/** Sets the current value as float and triggers update notification. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetFloatValue(float NewValue);

	/** Sets the current value as double and triggers update notification. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetDoubleValue(double NewValue);

	/** Sets the current value as FString and triggers update notification. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "NewValue"))
	void SetStringValue(const FString& NewValue);
	
	/** Converts a raw stored string into display text. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "String"))
	FText ValueStringToText(const FString& String) const;
	
protected:
#if WITH_EDITOR
	/** Whether ValueType can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingValueType() const;

	/** Whether ConfigParams can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingConfigParams() const;

	/** Whether EditorPreviewValue can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingEditorPreviewValue() const;
#endif

protected:
	/** Set the value type for this item and refresh. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetValueType(EVSCommonSettingValueType::Type NewValueType);

#if WITH_EDITOR
	void SetEditorPreviewValueString(const FString& NewValue);
#endif
	
	/** Converts a typed value to string using the item's formatting rules. */
	template<typename T>
	FString GetStringFromValue(const T& Value) const;

	/** Converts a string to a typed value using the item's parsing rules. */
	template<typename T>
	T GetValueFromString(const FString& String) const;

private:
	FString GetUnionValueString(const TValueUnion& UnionValue) const;
	void SetUnionValueFromString(const FString& String, TValueUnion& UnionValue) const;
	
	template<typename T>
	void SetUnionValue(const T& NewValue, TValueUnion& UnionValue);

protected:
	/** Preferred value type used for storage and conversion. */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (EditCondition = "EditorAllowChangingValueType()"))
	TEnumAsByte<EVSCommonSettingValueType::Type> ValueType = EVSCommonSettingValueType::None;

public:
	/** Numeric display text format. Use `{0}` as the numeric placeholder. */
	UPROPERTY(EditAnywhere, Category = "Display")
	FText DisplayNumericTextFormat = FText::FromString("{0}");
	
	/** Fractional digit range used when formatting numeric display text. */
	UPROPERTY(EditAnywhere, Category = "Display")
	FIntPoint DisplayNumericFractionDigitRange = FIntPoint(0, 324);
	
	/** Multiplier applied before numeric value is converted to display text. */
	UPROPERTY(EditAnywhere, Category = "Display")
	double DisplayNumericValueMultiplier = 1.f;

private:
	TValueUnion CurrentValue = TValueUnion();
	TValueUnion ConfirmedValue = TValueUnion();

#if WITH_EDITORONLY_DATA
	/**
	 * Preview value that can be modified in editor.
	 * @remarks This is transient and will not be saved or loaded.
	 */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Value", Category = "Settings", meta = (EditCondition = "EditorAllowChangingEditorPreviewValue()"))
	FString EditorPreviewValue;

	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Text", Category = "Settings", Transient)
	FText EditorPreviewText;

	/** Cached preview input used to rollback invalid editor edits. */
	FString LastEditorPreviewValue;
#endif
};

template <typename T>
void UVSCommonSettingItem::SetUnionValue(const T& NewValue, TValueUnion& UnionValue)
{
	if constexpr (std::is_same_v<T, FString>)
	{
		/** String input is parsed into the current union subtype when possible. */
		switch (static_cast<EVSCommonSettingValueType::Type>(UnionValue.GetCurrentSubtypeIndex() + 1))
		{
		case EVSCommonSettingValueType::Boolean:
			UnionValue.SetSubtype<bool>(GetValueFromString<bool>(NewValue));
			break;
		
		case EVSCommonSettingValueType::Integer:
			UnionValue.SetSubtype<int32>(GetValueFromString<int32>(NewValue));
			break;
		
		case EVSCommonSettingValueType::LongInteger:
			UnionValue.SetSubtype<int64>(GetValueFromString<int64>(NewValue));
			break;
		
		case EVSCommonSettingValueType::Float:
			UnionValue.SetSubtype<float>(GetValueFromString<float>(NewValue));
			break;
		
		case EVSCommonSettingValueType::Double:
			UnionValue.SetSubtype<double>(GetValueFromString<double>(NewValue));
			break;
		
		case EVSCommonSettingValueType::None:
		case EVSCommonSettingValueType::String:
			UnionValue.SetSubtype<FString>(NewValue);
			break;
			
		default: ;
		}
	}
	else
	{
		/** Non-string input is coerced to the configured ValueType. */
		switch (ValueType)
		{
		case EVSCommonSettingValueType::Boolean:
			UnionValue.SetSubtype<bool>(static_cast<bool>(NewValue));
			break;
		
		case EVSCommonSettingValueType::Integer:
			UnionValue.SetSubtype<int32>(static_cast<int32>(NewValue));
			break;
		
		case EVSCommonSettingValueType::LongInteger:
			UnionValue.SetSubtype<int64>(static_cast<int64>(NewValue));
			break;
		
		case EVSCommonSettingValueType::Float:
			UnionValue.SetSubtype<float>(static_cast<float>(NewValue));
			break;
		
		case EVSCommonSettingValueType::Double:
			UnionValue.SetSubtype<double>(static_cast<double>(NewValue));
			break;
		
		case EVSCommonSettingValueType::None:
		case EVSCommonSettingValueType::String:
			UnionValue.SetSubtype<FString>(GetStringFromValue(NewValue));
			break;
			
		default: ;
		}
	}
}

template <typename T>
// ReSharper disable once CppNotAllPathsReturnValue
FString UVSCommonSettingItem::GetStringFromValue(const T& Value) const
{
	static FString EmptyString = FString();
	
	if constexpr (std::is_same_v<T, bool>)
	{
		return FString(Value ? "true" : "false");
	}
	else if constexpr (std::is_same_v<T, int32>)
	{
		return FString::FromInt(Value);
	}
	else if constexpr (std::is_same_v<T, int64>)
	{
		return FString::Printf(TEXT("%lld"), Value);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		return FString::Printf(TEXT("%f"), Value);
	}
	else if constexpr (std::is_same_v<T, double>)
	{
		return FString::Printf(TEXT("%lf"), Value);
	}
	else if constexpr (std::is_same_v<T, FString>)
	{
		return Value;
	}

	// return EmptyString;
}

template <typename T>
// ReSharper disable once CppNotAllPathsReturnValue
T UVSCommonSettingItem::GetValueFromString(const FString& String) const
{
	static T EmptyT = T();

	if constexpr (std::is_same_v<T, bool>)
	{
		return FCString::ToBool(*String);
	}
	else if constexpr (std::is_same_v<T, int32>)
	{
		return FCString::Atoi(*String);
	}
	else if constexpr (std::is_same_v<T, int64>)
	{
		return FCString::Atoi64(*String);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		return FCString::Atof(*String);
	}
	else if constexpr (std::is_same_v<T, double>)
	{
		return FCString::Atod(*String);
	}
	else if constexpr (std::is_same_v<T, FString>)
	{
		return String;
	}

	// return EmptyT;
}
