// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_ScalabilityQualityLevel.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Graphics.Scalability.QualityLevel")
class VSSETTINGSYSTEM_API UVSSettingItem_ScalabilityQualityLevel : public UVSConsoleVariableSettingItem
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
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;

#if WITH_EDITOR
	virtual bool AllowChangingValueType_Implementation() const override { return false; }
	virtual bool AllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowChangingEditorPreviewValue_Implementation() const override { return false; }
	virtual bool AllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualityLevel(EPerQualityLevels InQualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EPerQualityLevels GetQualityLevel(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

private:
#if WITH_EDITOR
	TMap<FGameplayTag, FText> GetScalabilityQualityLevelDefaultDisplayNames() const;
	TMap<FGameplayTag, FString> GetScalabilityQualityLevelConsoleVariableNames() const;
#endif
	
protected:
#if WITH_EDITORONLY_DATA
	/** Preview window mode that can be modified in editor. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Quality Level", Category = "Settings", Transient, SkipSerialization)
	EPerQualityLevels EditorPreviewQualityLevel;
#endif
};
