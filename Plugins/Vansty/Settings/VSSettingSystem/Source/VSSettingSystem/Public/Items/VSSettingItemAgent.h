// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItem.h"
#include "VSSettingItemAgent.generated.h"

/**
 * Setting item that aggregates and drives a set of sub setting items.
 *
 * Acts as a composite entry that forwards lifecycle operations (load/validate/apply/confirm/save)
 * to its owned sub items, allowing multiple settings to be managed as a single unit.
 */
UCLASS(DisplayName = "VS.Settings.Item.Agent")
class VSSETTINGSYSTEM_API UVSSettingItemAgent : public UVSSettingItem
{
	GENERATED_UCLASS_BODY()

	friend class UVSSettingSubsystem;

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItemBase Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;

	virtual void Load_Implementation() override;
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	//~ End UVSSettingItemBase Interface

	TArray<UVSSettingItem*> GetDirectSubSettingItems() const;
	TArray<UVSSettingItem*> GetRecursiveSubSettingItems() const;

private:
	/** Instanced child setting items driven by this agent. */
	UPROPERTY(EditAnywhere, Category = "Settings", Instanced, meta = (ConfigRestartRequired = "true"))
	TArray<TObjectPtr<UVSSettingItem>> SubSettingItems;

#if WITH_EDITOR
	TArray<TObjectPtr<UVSSettingItem>> EditorSubSettingItems;
#endif
};
