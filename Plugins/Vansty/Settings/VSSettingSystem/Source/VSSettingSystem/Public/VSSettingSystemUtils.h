// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/VSSettingItemBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSSettingSystemUtils.generated.h"

class UVSSettingSubsystem;
class UVSSettingItemBase;

/**
 * Blueprint utility library for settings.
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the engine-global settings subsystem instance. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static UVSSettingSubsystem* GetSettingSubsystemVS();

	
	/** Returns all registered setting items (root agents + sub items). */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static TArray<UVSSettingItemBase*> GetAllSettingItems();

	/** Returns tag->item map for all registered setting items. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static TMap<FGameplayTag, UVSSettingItemBase*> GetAllTaggedSettingItems();

	/** Returns all currently registered setting item identifiers. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static FGameplayTagContainer GetAllSettingItemIdentifiers();

	
	/** Returns a registered setting item by identifier, or nullptr if not found. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Identifier"))
	static UVSSettingItemBase* GetSettingItemByIdentifier(const FGameplayTag& Identifier);

	/** Returns setting items whose identifiers match the provided identifier tag. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Identifier"))
	static TArray<UVSSettingItemBase*> GetSettingItemsByIdentifierMatching(const FGameplayTag& Identifier);

	/** Returns setting items whose item tags satisfy the provided gameplay tag query. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Query"))
	static TArray<UVSSettingItemBase*> GetSettingItemsByTagQuery(const FGameplayTagQuery& Query);

	/** Returns root-most items from the provided item list. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Items"))
	static TArray<UVSSettingItemBase*> FilterRootSettingItems(const TArray<UVSSettingItemBase*>& Items);


	/** Returns identifiers for all registered setting items that match the provided tag. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Identifier"))
	static FGameplayTagContainer GetSettingItemIdentifiersByMatching(const FGameplayTag& Identifier);
	
	/** Returns identifiers for all registered setting items that satisfy the given gameplay tag query. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Query"))
	static FGameplayTagContainer GetSettingItemIdentifiersByTagQuery(const FGameplayTagQuery& Query);
	
	/** Returns root-most identifiers from the provided identifier set. */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (AutoCreateRefTerm = "Identifiers"))
	static FGameplayTagContainer FilterRootSettingItemIdentifiers(const FGameplayTagContainer& Identifiers);


	/** Sets root-most items in the input list to values from the specified source. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Items"))
	static void SetSettingsToBySourceForItems(EVSSettingItemValueSource::Type ValueSource, const TArray<UVSSettingItemBase*>& Items);

	/** Executes one action for the provided items (with root filtering). */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Items"))
	static void ExecuteSettingActionForItems(EVSSettingItemAction::Type Action, const TArray<UVSSettingItemBase*>& Items);

	/** Executes actions for the provided items (with root filtering). */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions, Items"))
	static void ExecuteSettingActionsForItems(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions, const TArray<UVSSettingItemBase*>& Items);
	
	
	/** Resolves identifiers and sets root-most resolved items to values from the specified source. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Identifiers"))
	static void SetSettingsToBySourceForItemIdentifiers(EVSSettingItemValueSource::Type ValueSource, const FGameplayTagContainer& Identifiers);

	/** Executes one action for items resolved from identifiers (with root filtering). */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Identifiers"))
	static void ExecuteSettingActionForItemIdentifiers(EVSSettingItemAction::Type Action, const FGameplayTagContainer& Identifiers);

	/** Executes actions for items resolved from identifiers (with root filtering). */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions, Identifiers"))
	static void ExecuteSettingActionsForItemIdentifiers(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions, const FGameplayTagContainer& Identifiers);


	/** Executes a single action on all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void ExecuteSettingAction(EVSSettingItemAction::Type Action);
	
	/** Executes actions on all root agents in order. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	static void ExecuteSettingActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);
	
	/** Sets all registered settings to values from the specified source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SetSettingsToBySource(EVSSettingItemValueSource::Type ValueSource);
	
	/** Loads settings through the subsystem if available. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void LoadSettings();

	/** Validates settings through the subsystem if available. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void ValidateSettings();

	/** Applies settings through the subsystem if available. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void ApplySettings();

	/** Confirms settings through the subsystem if available. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void ConfirmSettings();

	/** Saves settings through the subsystem if available. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SaveSettings();
};
