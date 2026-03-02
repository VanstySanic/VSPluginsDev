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
 * Builds the runtime setting item registry from configured root agents, provides tag-based lookup,
 * and exposes batch operations (load/validate/apply/confirm/save) for the entire setting tree.
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
	
	/** Returns a registered setting item by tag, or nullptr if not found. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "ItemTag"))
	UVSSettingItemBase* GetSettingItemByTag(const FGameplayTag& ItemTag);

	/** Loads settings for all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();
	
	/** Validates settings for all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ValidateSettings();

	/** Applies settings for all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();
	
	/** Confirms settings for all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ConfirmSettings();

	/** Saves settings for all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();
	
	/** Executes a single action on all root agents. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteSettingAction(EVSSettingItemAction::Type Action);
	
	/** Executes actions on all root agents in order. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteSettingActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);

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
};
