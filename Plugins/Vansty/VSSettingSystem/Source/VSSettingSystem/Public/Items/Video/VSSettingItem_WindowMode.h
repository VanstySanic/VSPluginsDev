// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_WindowMode.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingItem_WindowMode : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void SetToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool EqualsToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetWindowMode(EWindowMode::Type InWindowMode);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EWindowMode::Type GetWindowMode(EVSSettingItemValueType::Type ValueType = EVSSettingItemValueType::Settings) const;
};
