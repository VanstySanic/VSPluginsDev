// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingItem.h"
#include "VSSettingItem_ScreenResolution.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Video.ScreenResolution")
class VSSETTINGSYSTEM_API UVSSettingItem_ScreenResolution : public UVSCommonSettingItem
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
	
	//~ Begin UVSSettingItem Interface
	virtual void Initialize_Implementation() override;
	virtual void OnValueUpdated_Implementation() override;
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual FString GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;

#if WITH_EDITOR
	virtual bool AllowChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowChangingValueType_Implementation() const override { return false; }
	virtual bool AllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowChangingEditorPreviewValue_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetScreenResolution(const FIntPoint& InResolution);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FIntPoint GetScreenResolution(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", Transient, SkipSerialization)
	FIntPoint EditorPreviewScreenResolution = FIntPoint::ZeroValue;
#endif
};
