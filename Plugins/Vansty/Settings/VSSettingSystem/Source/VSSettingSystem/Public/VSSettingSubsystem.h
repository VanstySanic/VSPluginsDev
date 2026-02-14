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
 * In editor builds, it also supports registering/unregistering transient setting items and tracking
 * tag changes for live iteration.
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FSettingItemUpdateDelegate, UVSSettingItemBase* /** SettingItem */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemUpdateEvent, UVSSettingItemBase*, SettingItem);
	
public:
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
	/** Removes previously registered root agents from the editor registry. */
	void ClearEditorDirectSettingItemAgents();
	void RefreshEditorDirectSettingItemAgents();
#endif

public:
	FSettingItemUpdateDelegate OnItemUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemUpdateEvent OnItemUpdated;

private:
	void OnSettingItemUpdated(UVSSettingItemBase* SettingItem);

private:
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItemAgent>> DirectSettingItemAgents;
	
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItemBase>> SettingItems;
	
	TMap<FGameplayTag, TWeakObjectPtr<UVSSettingItemBase>> TaggedSettingItems;
};