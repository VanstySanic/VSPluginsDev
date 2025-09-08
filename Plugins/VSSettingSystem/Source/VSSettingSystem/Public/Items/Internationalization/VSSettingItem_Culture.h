// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_Culture.generated.h"

/**
 * Includes language and locale.
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingItem_Culture : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetCulture(const FString& InCulture);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetCulture(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

protected:

private:
	FString LastConfirmedCulture;
};
