// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "VSEnhancedInputMappingKeySettingItem.generated.h"

USTRUCT(BlueprintType)
struct FVSEnhancedInputMappingKeySlot
{
	GENERATED_BODY()
	
	FVSEnhancedInputMappingKeySlot(EPlayerMappableKeySlot Slot = EPlayerMappableKeySlot::First)
		: Slot(Slot)
	{
	}

	FVSEnhancedInputMappingKeySlot(int32 Slot)
		: Slot(EPlayerMappableKeySlot(Slot))
	{
	}

	bool operator==(const FVSEnhancedInputMappingKeySlot& Other) const
	{
		return Slot == Other.Slot;
	}

	friend uint32 GetTypeHash(const FVSEnhancedInputMappingKeySlot& Item)
	{
		return GetTypeHash(Item.Slot);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerMappableKeySlot Slot = EPlayerMappableKeySlot::First;
};


/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSEnhancedInputMappingKeySettingItem : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItem Interface
	virtual void Load_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	TMap<FVSEnhancedInputMappingKeySlot, FKey> GetKeys(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FKey GetKey(const FVSEnhancedInputMappingKeySlot& Slot, EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SlottedKeys"))
	void SetKeys(const TMap<FVSEnhancedInputMappingKeySlot, FKey>& SlottedKeys);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetKey(const FVSEnhancedInputMappingKeySlot& Slot, const FKey& Key);


private:
	UFUNCTION()
	TArray<FName> GetMappingNames() const;
	
	UEnhancedInputUserSettings* GetEnhancedInputUserSettings() const;
	FMapPlayerKeyArgs MakeMapPlayerKeyArgs(FVSEnhancedInputMappingKeySlot Slot, FKey Key) const;
	TMap<FVSEnhancedInputMappingKeySlot, FKey> GetKeysFromUserSettings(bool bUseDefault) const;
	TMap<FVSEnhancedInputMappingKeySlot, FKey> GetKeysFromMappingContext() const;

protected:
	/** The Enhanced input mapping context to get keys from and bind keys to. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	TObjectPtr<UInputMappingContext> MappingContext;
	
	/** The Key Mapping Profile identifier that this mapping should be set on. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	FGameplayTag ProfileId;

	/**
	 * The name of the mapping for this key. This is either the default mapping name from an Input Action asset,
	 * or one that is overridden in the Input Mapping Context.
	 */
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (GetOptions = "GetMappingNames"))
	FName MappingName = NAME_None;

	/** An OPTIONAL specifier about what kind of hardware this mapping is for. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetOptions = "Engine.InputPlatformSettings.GetAllHardwareDeviceNames"))
	FName HardwareDeviceID = NAME_None;

	/** Apply to specified local player. If none, apply to 0 by default. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 LocalPlayerIndex = 0;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Slotted Keys", Category = "Settings")
	TMap<FVSEnhancedInputMappingKeySlot, FKey> EditorPreviewSlottedKeys;
	
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Default Keys", Category = "Settings")
	TMap<FVSEnhancedInputMappingKeySlot, FKey> EditorPreviewDefaultKeys;
#endif

	UPROPERTY()
	TMap<FVSEnhancedInputMappingKeySlot, FKey> CurrentSlottedKeys;
	TMap<FVSEnhancedInputMappingKeySlot, FKey> AppliedSlottedKeys;
	TMap<FVSEnhancedInputMappingKeySlot, FKey> ConfirmedSlottedKeys;
};
