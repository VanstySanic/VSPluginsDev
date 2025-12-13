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
	FText DisplayName = FText::FromString("None");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

UENUM(BlueprintType)
namespace EVSSettingItemAction
{
	enum Type
	{
		None					UMETA(Hidden),
		
		SetToDefault,
		SetToGame,
		SetToLastConfirmed,
		
		Load,
		Validate,
		Apply,
		Confirm,
		Save
	};
}

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

		/** Value that are confirmed from setting system's. */
		Confirmed
	};
}

/**
 * 
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
	virtual void PreEditChange(class FEditPropertyChain& PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void BeginDestroy() override;
	//~ End UObject Interface
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasBeenInitialized() const { return bHasBeenInitialized; }

	UFUNCTION(BlueprintCallable, Category = "Feature")
	FGameplayTag GetItemTag() const { return ItemTag; }
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	FVSSettingItemInfo GetItemInfo() const { return ItemInfo; }
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Load();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Validate();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Apply();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Confirm();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Save();


	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDirty() const;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsUnconfirmed() const;
	
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void SetToBySource(const EVSSettingItemValueSource::Type ValueSource);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EqualsToBySource(const EVSSettingItemValueSource::Type ValueSource) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteAction(EVSSettingItemAction::Type Action);
	
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);

	/** Notify the update of this item. Needs to call manually. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void NotifyValueUpdate(bool bAllowCleanNotify = false);

protected:
#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool AllowChangingItemTag() const;
#endif
	
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Initialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Uninitialize();
	
public:
	FSettingItemDelegate OnUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemEvent OnUpdated;
	
protected:
	/** Unique gameplay tag that identifies a single setting item.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSSettingItemInfo ItemInfo;

	/** Executed when the value is updated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<TEnumAsByte<EVSSettingItemAction::Type>> ValueUpdatedActions;
	
private:
	bool bHasBeenInitialized = false;
};
