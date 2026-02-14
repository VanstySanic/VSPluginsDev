// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSSettingItemBase.generated.h"

USTRUCT(BlueprintType)
struct FVSSettingItemInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

/** High-level actions that can be executed on a setting item. */
UENUM(BlueprintType)
namespace EVSSettingItemAction
{
	enum Type
	{
		None					UMETA(Hidden),
		
		SetToDefault,
		SetToGame,
		SetToConfirmed,
		
		Load,
		Apply,
		Confirm,
		Save
	};
}

/** Defines which value source a setting item should compare against or apply from. */
UENUM(BlueprintType)
namespace EVSSettingItemValueSource
{
	enum Type
	{
		None		UMETA(Hidden),

		/** The engine's default value. */
		Default,
		
		/** The real value set on your game. This could be different form the setting system value.*/
		Game,
		
		/** Value in the setting system. This could be different form the current value. */
		System,
		
		/** Value that is confirmed from setting system's. */
		Confirmed
	};
}

USTRUCT(BlueprintType)
struct FVSSettingItemConfigSettings
{
	GENERATED_USTRUCT_BODY()

	FVSSettingItemConfigSettings()
		// : bDesireManualConfig(false)
	{
	}
	
	/**
	 * Target config file (without extension), e.g. "Editor" and "GameUserSettings".
	 * @remark In some items, the file name appears as "Editor" in editor,
	 * but this will be "GameUserSettings" in non-editor game.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileName = FString("GameUserSettings");

	/** Config section name used to store the value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Section = FString("VS.SettingSystem.Item");

	/** Does not work when empty or none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PrimaryKey = FString("None");
	
	/** Config keys used to store the value. This should be handled by user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> AdditionalNamedKeys;

	// /** If true, config process is desired to be handled by user. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// uint8 bDesireManualConfig : 1;
};

/**
 * Base UObject representing a single configurable setting item.
 *
 * Encapsulates the lifecycle, state comparison, and action execution logic
 * for a setting entry, including loading, validation, application, and
 * confirmation. Each setting item is identified by a gameplay tag and can
 * dispatch update notifications to interested systems.
 *
 * This class is designed to be extended for concrete setting implementations,
 * while higher-level systems coordinate persistence and synchronization.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DisplayName = "VS.Settings.Item.Base")
class VSSETTINGSYSTEM_API UVSSettingItemBase : public UObject
{
GENERATED_UCLASS_BODY()
	
	friend class UVSSettingItemAgent;
	friend class UVSSettingSubsystem;

	DECLARE_MULTICAST_DELEGATE_OneParam(FSettingItemDelegate, UVSSettingItemBase* /** SettingItem */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemEvent, UVSSettingItemBase*, SettingItem);
	
public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void BeginDestroy() override;
	//~ End UObject Interface
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool HasBeenInitialized() const { return bHasBeenInitialized; }

	/** Gameplay tag uniquely identifying this setting item. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FGameplayTag GetItemTag() const { return ItemTag; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FVSSettingItemInfo GetItemInfo() const { return ItemInfo; }

	/** Load value from persistent storage or external source. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void Load();

	/** Apply the current value to the game runtime. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void Apply();
	
	/** Confirm the current value as accepted. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void Confirm();

	/** Save the current value to persistent storage. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void Save();

	/** Validate current value without applying it. Call it in proper places. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void Validate();

	/** Whether the value in this item is valid. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool IsValueValid() const;

	/** Set the value to the default value. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetToDefault();

	/** Set the value to the current game value. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetToGame();

	/** Set the value to last confirmed. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetToConfirmed();
	
	/** Set value based on the specified source. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void SetToBySource(const EVSSettingItemValueSource::Type ValueSource);
	
	/** Compare current value against the specified source. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EqualsToBySource(const EVSSettingItemValueSource::Type ValueSource) const;
	
	/** Whether the system value differs from the confirmed value. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDirty() const;
	
	/** Whether the current value has not yet been confirmed. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsUnconfirmed() const;

	
	/** Execute a single predefined setting action. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteAction(EVSSettingItemAction::Type Action);
	
	/** Execute multiple predefined setting actions in order. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);

	/**
	 * Notify listeners that the value has changed.
	 * @param bAllowCleanNotify Whether to notify even if the value is considered clean.
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void NotifyValueUpdated(bool bAllowCleanNotify = false);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void NotifyValueExternChanged(bool bAllowSameNotify = false);

	/** Defines whether the setting item will be loaded and handled in the subsystem. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	bool ShouldCreateSettingItem() const;

protected:
	/** Initialization should be executed before any other actions. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Initialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Uninitialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnValueUpdated();

#if WITH_EDITOR
	/** Whether the ItemTag can be modified in the editor. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingItemTag() const;
#endif
	
public:
	FSettingItemDelegate OnUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemEvent OnUpdated;
	
protected:
	/** Unique gameplay tag that identifies this setting item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (EditCondition = "EditorAllowChangingItemTag()"))
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSSettingItemInfo ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSSettingItemConfigSettings ConfigSettings;

	/** Execute when the game value changes inside the item. Runtime only. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TArray<TEnumAsByte<EVSSettingItemAction::Type>> InternalChangeActions;
	
	/** Execute when the game value changes outside the item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TArray<TEnumAsByte<EVSSettingItemAction::Type>> ExternalChangeActions;

#if WITH_EDITORONLY_DATA
	/** Execute when the game value changes inside the item when in editor details. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TArray<TEnumAsByte<EVSSettingItemAction::Type>> EditorChangeActions;
#endif
	
private:
	bool bHasBeenInitialized = false;
};