// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingSystemUtils.generated.h"

struct FGameplayTagQuery;
struct FGameplayTag;
class UVSSettingItemBase;

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<UVSSettingItemBase*> GetSettingItems() const;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "IdentifyTag"))
	UVSSettingItemBase* GetSettingItemByIdentifyTag(const FGameplayTag& IdentifyTag) const;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "CategoryTag"))
	TArray<UVSSettingItemBase*> GetSettingItemsByCategoryTag(const FGameplayTag& CategoryTag) const;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "TagQuery"))
	TArray<UVSSettingItemBase*> GetSettingItemsByTagQuery(const FGameplayTagQuery& TagQuery) const;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SettingItems, Actions"))
	void ExecuteActionForSettingItems(const TArray<UVSSettingItemBase*>& SettingItems, const TEnumAsByte<EVSSettingItemAction::Type> Action);
	
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SettingItems, Actions"))
	void ExecuteActionsForSettingItems(const TArray<UVSSettingItemBase*>& SettingItems, const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);
};
