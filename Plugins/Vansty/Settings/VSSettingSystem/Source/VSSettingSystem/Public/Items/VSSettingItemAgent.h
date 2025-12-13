// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItem.h"
#include "VSSettingItemAgent.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.Settings.Item.Agent")
class VSSETTINGSYSTEM_API UVSSettingItemAgent : public UVSSettingItem
{
	GENERATED_UCLASS_BODY()
	
public:
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	
	//~ Begin UVSSettingItemBase Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Load_Implementation() override;
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	//~ End UVSSettingItemBase Interface
	
	TArray<UVSSettingItem*> GetDirectSubSettingItems() const { return SubSettingItems; };

private:
	UPROPERTY(EditAnywhere, Category = "Settings", Instanced, meta = (ConfigRestartRequired = "true"))
	TArray<TObjectPtr<UVSSettingItem>> SubSettingItems;

#if WITH_EDITOR
	TArray<TObjectPtr<UVSSettingItem>> EditorSubSettingItems;
#endif
};
