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
protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	
public:
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void OnValueUpdated_Implementation() override;
	virtual FString GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;
	
protected:
#if WITH_EDITOR
	virtual void EditorPostInitialized_Implementation() override;
	virtual bool EditorAllowChangingItemTag_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingEditorPreviewValue_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetScreenResolution(const FIntPoint& InResolution);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FIntPoint GetScreenResolution(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "String"))
	FIntPoint StringToScreenResolution(const FString& String) const;

private:
	void OnSystemResolutionChanged(uint32 ResX, uint32  ResY);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bCheckForCommandLineOverrides : 1;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", Transient)
	FIntPoint EditorPreviewScreenResolution = FIntPoint::ZeroValue;
#endif
};
