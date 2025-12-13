// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/VSSettingItem.h"
#include "Subsystems/EngineSubsystem.h"
#include "VSSettingSubsystem.generated.h"

struct FGameplayTag;
class UVSSettingItemAgent;
class UVSSettingItem;

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UVSSettingSubsystem* Get();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "ItemTag"))
	UVSSettingItem* GetSettingItemByTag(const FGameplayTag& ItemTag = FGameplayTag());

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ValidateSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ConfirmSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteSettingAction(EVSSettingItemAction::Type Action);
	
	/** Execute actions on all setting items. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteSettingActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);

#if WITH_EDITOR
	void AddEditorSettingItem(UVSSettingItem* SettingItem);
	void RemoveEditorSettingItem(UVSSettingItem* SettingItem);
	void ReregisterEditorSettingItem(UVSSettingItem* SettingItem);
#endif

private:
	void InitSettingItems();

private:
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItemAgent>> DirectSettingItemAgents;
	
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItem>> SettingItems;

	TMap<FGameplayTag, TWeakObjectPtr<UVSSettingItem>> TaggedSettingItems;

#if WITH_EDITOR
	TMap<TWeakObjectPtr<UVSSettingItem>, FGameplayTag> EditorSettingItemTags;
	uint8 bEditorHasBeenInitialized : 1;
#endif
};
