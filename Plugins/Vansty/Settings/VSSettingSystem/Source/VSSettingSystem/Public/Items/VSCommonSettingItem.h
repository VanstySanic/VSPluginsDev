// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItem.h"
#include "Containers/Union.h"
#include "VSCommonSettingItem.generated.h"

UENUM(BlueprintType)
namespace EVSCommonSettingValueType
{
	enum Type
	{
		None		UMETA(Hidden),
		Boolean,
		Integer,
		LongInteger,
		Float,
		Double,
		String,
	};
}

USTRUCT(BlueprintType)
struct FVSCommonSettingConfigParams
{
	GENERATED_USTRUCT_BODY()

	FVSCommonSettingConfigParams()
		: bAutoConfig(true)
	{
	}
	
	UPROPERTY(EditAnywhere)
	FString ConfigFileName = FString("GameUserSettings");

	UPROPERTY(EditAnywhere)
	FString ConfigSection = FString("VSSettingSystem");

	UPROPERTY(EditAnywhere)
	FString ConfigKeyName;

	/** If true, the config save and load will be processed, otherwise user should do config manually. */
	UPROPERTY(EditAnywhere)
	uint8 bAutoConfig : 1;
};

/**
 * 
 */
UCLASS(Abstract, DisplayName = "VS.Settings.Item.Common")
class VSSETTINGSYSTEM_API UVSCommonSettingItem : public UVSSettingItem
{
	GENERATED_UCLASS_BODY()

	using TValueUnion = TUnion<bool, int32, int64, float, double, FString>;
	
public:
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UVSSettingItemBase Interface
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void Load_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void Confirm_Implementation() override;
	//~ End UVSSettingItemBase Interface

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	bool GetBooleanValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	int32 GetIntegerValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	int64 GetLongIntegerValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	float GetFloatValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	double GetDoubleValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	FString GetStringValue(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;


	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetBooleanValue(bool bNewValue);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetIntegerValue(int32 NewValue);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetLongIntegerValue(int64 NewValue);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetFloatValue(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetDoubleValue(double NewValue);

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "NewValue"))
	void SetStringValue(const FString& NewValue);
	
protected:
#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool AllowChangingValueType() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool AllowChangingConfigParams() const;
#endif

private:
#if WITH_EDITOR
	void RefreshValueType();
#endif
	FString GetUnionValueString(const TValueUnion& UnionValue);
	void SetUnionValueFromString(const FString& String, TValueUnion& UnionValue);
	
	template<typename T>
	void SetUnionValue(const T& NewValue, TValueUnion& UnionValue);
	
	template<typename T>
	FString GetStringFromValue(const T& Value) const;

	template<typename T>
	T GetValueFromString(const FString& String) const;

	
protected:
	/** The most preferred value type for the item. */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (EditCondition = "AllowChangingValueType()"))
	TEnumAsByte<EVSCommonSettingValueType::Type> ValueType = EVSCommonSettingValueType::None;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (EditCondition = "AllowChangingConfigParams()"))
	FVSCommonSettingConfigParams ConfigParams;
	
private:
	TValueUnion CurrentValue;
	TValueUnion ConfirmedValue;

#if WITH_EDITORONLY_DATA
	TEnumAsByte<EVSCommonSettingValueType::Type> LastEditorValueType = EVSCommonSettingValueType::None;
	FVSCommonSettingConfigParams LastEditorConfigParams = FVSCommonSettingConfigParams();
#endif
};


template <typename T>
void UVSCommonSettingItem::SetUnionValue(const T& NewValue, TValueUnion& UnionValue)
{
	if constexpr (std::is_same_v<T, FString>)
	{
		switch (static_cast<EVSCommonSettingValueType::Type>(UnionValue.GetCurrentSubtypeIndex() + 1)) {
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
		switch (ValueType) {
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
