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
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItem Interface
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void OnValueUpdated_Implementation() override;
	virtual FString GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;

#if WITH_EDITOR
	virtual bool AllowEditorChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowEditorChangingValueType_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowEditorChangingEditorPreviewValue_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetScreenResolution(const FIntPoint& InResolution);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FIntPoint GetScreenResolution(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", Transient)
	FIntPoint EditorPreviewScreenResolution = FIntPoint::ZeroValue;
#endif
};
