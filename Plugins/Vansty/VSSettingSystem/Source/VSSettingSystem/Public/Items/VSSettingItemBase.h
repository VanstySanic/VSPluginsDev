// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/VSSettingSystemTypes.h"
#include "UObject/Object.h"
#include "VSSettingItemBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew, DisplayName = "Settings.Item.Base")
class VSSETTINGSYSTEM_API UVSSettingItemBase : public UObject
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingItemUpdateSignature);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemActionSignature, EVSSettingItemAction::Type, Action);


public:
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Uninitialize();


	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteAction(TEnumAsByte<EVSSettingItemAction::Type> Action);

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void NotifyUpdate();
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Load();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void SetToByValueType(const EVSSettingItemValueType::Type ValueType);

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Validate();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Apply();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Confirm();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Save();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EqualsToByValueType(const EVSSettingItemValueType::Type ValueType) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDirty() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDefault() const;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsUnconfirmed() const;
	
public:
	/** Get the item tags that also includes the IdentityTag and CategoryTag. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FGameplayTagContainer GetSettingItemTags();

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemUpdateSignature OnUpdated;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemActionSignature OnActionExecuted;
	
public:
	UPROPERTY(EditAnywhere, Category = "Settings|Tag")
	FGameplayTag IdentityTag;

	UPROPERTY(EditAnywhere, Category = "Settings|Tag")
	FGameplayTag CategoryTag;

	UPROPERTY(EditAnywhere, Category = "Settings|Tag")
	FGameplayTagContainer ItemTags;
};
