// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/VSSettingItemBase.h"
#include "Subsystems/EngineSubsystem.h"
#include "VSSettingSubsystem.generated.h"

struct FGameplayTag;
class UVSSettingItemAgent;
class UVSSettingItemBase;

/**
 * Engine subsystem that owns and coordinates all setting items.
 *
 * Builds and owns the runtime setting item registry from configured root agents.
 * Global batch operations live here, while item-scoped helper APIs are exposed by UVSSettingSystemUtils.
 *
 * In editor builds, it also supports clearing/rebuilding direct agents for live iteration.
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FSettingItemUpdateDelegate, UVSSettingItemBase* /** SettingItem */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemUpdateEvent, UVSSettingItemBase*, SettingItem);
	
public:
	/** Returns the engine-global setting subsystem instance, or nullptr when engine is unavailable. */
	static UVSSettingSubsystem* Get();

	//~ Begin UEngineSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void BeginDestroy() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End UEngineSubsystem Interface

	/** Returns a snapshot array of all currently registered setting items (root agents + sub items). */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<UVSSettingItemBase*> GetSettingItems() const;

	/** Returns a snapshot map from item identifier tag to registered setting item. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	TMap<FGameplayTag, UVSSettingItemBase*> GetTaggedSettingItems() const;

	/** Returns a registered setting item by identifier, or nullptr if not found. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Identifier"))
	UVSSettingItemBase* GetSettingItemByIdentifier(const FGameplayTag& Identifier) const;

	/** Returns the cached set of all currently registered item identifiers. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FGameplayTagContainer GetSettingItemIdentifiers() const;

	
	/** Executes one action on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteSettingAction(EVSSettingItemAction::Type Action);
	
	/** Executes multiple actions in order on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteSettingActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);

	/** Sets all registered root-agent trees to values from the specified source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSettingsToBySource(EVSSettingItemValueSource::Type ValueSource);
	
	/** Calls Load on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

	/** Calls Validate on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ValidateSettings();

	/** Calls Apply on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();

	/** Calls Confirm on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ConfirmSettings();

	/** Calls Save on every registered root agent. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	
	/** Registers additional root agent classes and integrates their setting trees into the registry. */
	void AddDirectSettingItemAgentClasses(const TArray<TSoftClassPtr<UVSSettingItemAgent>>& SettingItemAgentClasses);

#if WITH_EDITOR
	/** Clears all current direct agents/items and unbinds update delegates in editor-only refresh flow. */
	void ClearEditorDirectSettingItemAgents();
	/** Rebuilds direct agents/items from config for editor live iteration. */
	void RefreshEditorDirectSettingItemAgents();
#endif
	
private:
	void OnSettingItemUpdated(UVSSettingItemBase* SettingItem);
	
public:
	/** Broadcast whenever any registered setting item broadcasts an update. */
	FSettingItemUpdateDelegate OnItemUpdated_Native;
	
	/** Broadcast whenever any registered setting item broadcasts an update. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemUpdateEvent OnItemUpdated;

private:
	/** Root agents currently registered in this subsystem. */
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItemAgent>> DirectSettingItemAgents;
	
	/** Flat cache of all registered setting items (including root agents and sub items). */
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItemBase>> SettingItems;
	
	/** Fast lookup table from valid item tag to setting item. */
	TMap<FGameplayTag, TWeakObjectPtr<UVSSettingItemBase>> TaggedSettingItems;

	/** Cached container of all valid registered item identifiers. */
	UPROPERTY()
	FGameplayTagContainer AllSettingItemIdentifiers;
};
