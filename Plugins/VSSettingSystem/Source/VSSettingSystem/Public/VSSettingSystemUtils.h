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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	static TArray<UVSSettingItemBase*> GetSettingItems();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "SpecifyTag"))
	static UVSSettingItemBase* GetSettingItemBySpecifyTag(const FGameplayTag& SpecifyTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "CategoryTag"))
	static TArray<UVSSettingItemBase*> GetSettingItemsByCategoryTag(const FGameplayTag& CategoryTag, bool bExact = false);

	UFUNCTION(BlueprintCallable,BlueprintPure,  Category = "Settings", meta = (AutoCreateRefTerm = "TagQuery"))
	static TArray<UVSSettingItemBase*> GetSettingItemsByTagQuery(const FGameplayTagQuery& TagQuery);

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SettingItems, Actions"))
	static void ExecuteActionForSettingItems(const TArray<UVSSettingItemBase*>& SettingItems, const TEnumAsByte<EVSSettingItemAction::Type> Action);
	
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SettingItems, Actions"))
	static void ExecuteActionsForSettingItems(const TArray<UVSSettingItemBase*>& SettingItems, const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);
};
