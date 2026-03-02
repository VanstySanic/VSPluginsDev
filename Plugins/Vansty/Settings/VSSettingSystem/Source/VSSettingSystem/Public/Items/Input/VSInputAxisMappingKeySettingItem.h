// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSInputAxisMappingKeySettingItem.generated.h"

struct FInputAxisKeyMapping;
class UInputSettings;
class UPlayerInput;

USTRUCT(BlueprintType)
struct FVSInputMappingAxisSlot
{
	GENERATED_BODY()

	FVSInputMappingAxisSlot(int32 Index = 0)
		: Index(Index)
	{
	}

	bool operator==(const FVSInputMappingAxisSlot& Other) const
	{
		return Index == Other.Index;
	}

	friend uint32 GetTypeHash(const FVSInputMappingAxisSlot& Item)
	{
		return GetTypeHash(Item.Index);
	}

	/** Legacy axis mapping slot index wrapper. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;
};

/**
 * Legacy input axis-mapping key setting item.
 *
 * Manages slotted axis keys and applies them through `UPlayerInput`.
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSInputAxisMappingKeySettingItem : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
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
	/** Returns slotted axis keys for the requested source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	TMap<FVSInputMappingAxisSlot, FKey> GetKeys(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Returns axis key at a specific slot for the requested source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FKey GetKey(const FVSInputMappingAxisSlot& Slot, EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Replaces all current slotted axis keys. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SlottedKeys"))
	void SetKeys(const TMap<FVSInputMappingAxisSlot, FKey>& SlottedKeys);
	
	/** Sets/replaces one axis key slot. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetKey(const FVSInputMappingAxisSlot& Slot, const FKey& Key);

private:
	FInputAxisKeyMapping MakeAxisMapping(const FKey& Key, float Scale) const;
	float GetDesiredScaleForSlot(int32 SlotIndex, const UPlayerInput* PlayerInput) const;
	
	UPlayerInput* GetPlayerInput() const;
	TMap<FVSInputMappingAxisSlot, FKey> GetKeysFromPlayerInput(const UPlayerInput* PlayerInput) const;
	TMap<FVSInputMappingAxisSlot, FKey> GetKeysFromInputSettings(const UInputSettings* InputSettings) const;

protected:
	/** The axis name to get keys from and bind keys to. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName AxisName = NAME_None;

	/** Apply to specified local player. If none, apply to 0 by default. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 LocalPlayerIndex = 0;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Slotted Keys", Category = "Settings")
	TMap<FVSInputMappingAxisSlot, FKey> EditorPreviewSlottedKeys;
	
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Default Keys", Category = "Settings")
	TMap<FVSInputMappingAxisSlot, FKey> EditorPreviewDefaultKeys;
#endif

	UPROPERTY()
	TMap<FVSInputMappingAxisSlot, FKey> CurrentSlottedKeys;
	TMap<FVSInputMappingAxisSlot, FKey> AppliedSlottedKeys;
	TMap<FVSInputMappingAxisSlot, FKey> ConfirmedSlottedKeys;
};
