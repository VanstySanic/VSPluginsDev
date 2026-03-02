// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/InputChord.h"
#include "Items/VSSettingItemBase.h"
#include "VSInputActionMappingKeySettingItem.generated.h"

struct FInputActionKeyMapping;
class UInputSettings;
class UPlayerInput;

USTRUCT(BlueprintType)
struct FVSInputMappingKeySlot
{
	GENERATED_BODY()

	FVSInputMappingKeySlot(int32 Index = 0)
		: Index(Index)
	{
	}

	bool operator==(const FVSInputMappingKeySlot& Other) const
	{
		return Index == Other.Index;
	}

	friend uint32 GetTypeHash(const FVSInputMappingKeySlot& Item)
	{
		return GetTypeHash(Item.Index);
	}

	/** Legacy input mapping slot index wrapper. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;
};

/**
 * Legacy input action-mapping key setting item.
 *
 * Manages slotted action chords and applies them through `UPlayerInput`.
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSInputActionMappingKeySettingItem : public UVSSettingItemBase
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
	/** Returns slotted action chords for the requested source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	TMap<FVSInputMappingKeySlot, FInputChord> GetKeys(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Returns action chord at a specific slot for the requested source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FInputChord GetKey(const FVSInputMappingKeySlot& Slot, EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	/** Replaces all current slotted action chords. */
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "SlottedChords"))
	void SetKeys(const TMap<FVSInputMappingKeySlot, FInputChord>& SlottedChords);

	/** Sets/replaces one action chord slot. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetKey(const FVSInputMappingKeySlot& Slot, const FInputChord& Chord);

private:
	static FInputActionKeyMapping MakeActionMapping(FName MappingName, const FInputChord& Chord);
	static FInputChord MakeChord(const FInputActionKeyMapping& Mapping);

	UPlayerInput* GetPlayerInput() const;
	TMap<FVSInputMappingKeySlot, FInputChord> GetChordsFromPlayerInput() const;
	TMap<FVSInputMappingKeySlot, FInputChord> GetChordsFromInputSettings() const;

protected:
	/** The action name to get keys from and bind keys to. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName ActionName = NAME_None;

	/** Apply to specified local player. If none, apply to 0 by default. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 LocalPlayerIndex = 0;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Slotted Chords", Category = "Settings")
	TMap<FVSInputMappingKeySlot, FInputChord> EditorPreviewSlottedChords;
	
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Default Chords", Category = "Settings")
	TMap<FVSInputMappingKeySlot, FInputChord> EditorPreviewDefaultChords;
#endif

	UPROPERTY()
	TMap<FVSInputMappingKeySlot, FInputChord> CurrentSlottedChords;
	TMap<FVSInputMappingKeySlot, FInputChord> AppliedSlottedChords;
	TMap<FVSInputMappingKeySlot, FInputChord> ConfirmedSlottedChords;
};
