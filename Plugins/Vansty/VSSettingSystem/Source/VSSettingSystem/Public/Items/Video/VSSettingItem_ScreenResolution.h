// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_ScreenResolution.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Video.ScreenResolution")
class VSSETTINGSYSTEM_API UVSSettingItem_ScreenResolution : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnItemValueUpdated_Implementation() override;
	
	virtual void BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName = NAME_None) override;
	virtual void UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetScreenResolution(const FIntPoint& InResolution);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FIntPoint GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FText ResolutionToText(const FIntPoint& Resolution) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FIntPoint TextToResolution(const FText& Text) const;

private:
	UFUNCTION()
	void OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FText MiddleLinkText = FText::FromString(" x ");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bOptionsHighToLow = true;
};
