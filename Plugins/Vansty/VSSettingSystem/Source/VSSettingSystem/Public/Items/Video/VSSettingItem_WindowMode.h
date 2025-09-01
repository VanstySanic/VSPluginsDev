// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_WindowMode.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.WindowMode")
class VSSETTINGSYSTEM_API UVSSettingItem_WindowMode : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetWindowMode(EWindowMode::Type InWindowMode);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EWindowMode::Type GetWindowMode(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TMap<TEnumAsByte<EWindowMode::Type>, FText> WindowModeNames;
};
