// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItem_ScalabilityQualityLevel.h"
#include "VSSettingItem_ResolutionScale.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingItem_ResolutionScale : public UVSSettingItem_ScalabilityQualityLevel
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void SetToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) override;
	virtual void Apply_Implementation() override;
	virtual void Validate_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool EqualsToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetResolutionScale(float InResolutionScale);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetResolutionScale(const EVSSettingItemValueType::Type ValueType = EVSSettingItemValueType::Settings) const;

private:
	float ResolutionScale = 100.f;
	float LastConfirmedResolutionScale = 100.f;
};
