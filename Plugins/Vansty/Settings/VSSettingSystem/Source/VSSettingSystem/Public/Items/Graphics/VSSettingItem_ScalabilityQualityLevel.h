// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_ScalabilityQualityLevel.generated.h"

/**
 * Generic scalability quality-level setting item.
 *
 * Maps integer quality values to `EPerQualityLevels` for display and control.
 */
UCLASS(DisplayName = "VS.Settings.Item.Scalability.QualityLevel")
class VSSETTINGSYSTEM_API UVSSettingItem_ScalabilityQualityLevel : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;

	virtual void OnValueUpdated_Implementation() override;
	
#if WITH_EDITOR
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingEditorPreviewValue_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualityLevel(EPerQualityLevels InQualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EPerQualityLevels GetQualityLevel(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

private:
#if WITH_EDITOR
	static TMap<FGameplayTag, FText> GetScalabilityQualityLevelDefaultDisplayNames();
	static TMap<FGameplayTag, FString> GetScalabilityQualityLevelConsoleVariableNames();
#endif
	
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Quality Level", Category = "Settings", Transient)
	EPerQualityLevels EditorPreviewQualityLevel;
#endif
};
