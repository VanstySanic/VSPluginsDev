// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSSettingItem.generated.h"

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
class VSSETTINGSYSTEM_API UVSSettingItem : public UObject
{
GENERATED_UCLASS_BODY()
	
	friend class UVSSettingItemAgent;
	friend class UVSSettingSubsystem;

	DECLARE_MULTICAST_DELEGATE_OneParam(FSettingItemDelegate, UVSSettingItem* /** SettingItem */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemEvent, UVSSettingItem*, SettingItem);
	
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
	
	/** Whether the system value differs from the confirmed value. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDirty() const;
	
	/** Whether the current value has not yet been confirmed. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsUnconfirmed() const;

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
	void NotifyValueUpdate(bool bAllowCleanNotify = false);

protected:
#if WITH_EDITOR
	/** Whether the ItemTag can be modified in the editor. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool AllowEditorChangingItemTag() const;
#endif
	
	/** This is before any actions are executed. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Initialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Uninitialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void OnValueUpdated();

	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSSettingItemAgent* GetRootMostAgent() const;

public:
	FSettingItemDelegate OnUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemEvent OnUpdated;
	
protected:
	/** Unique gameplay tag that identifies this setting item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (EditCondition = "AllowEditorChangingItemTag()"))
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSSettingItemInfo ItemInfo;
	
private:
	bool bHasBeenInitialized = false;
};