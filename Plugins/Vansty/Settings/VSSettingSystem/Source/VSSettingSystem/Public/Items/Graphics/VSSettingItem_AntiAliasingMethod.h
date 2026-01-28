// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_AntiAliasingMethod.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Graphics.AntiAliasingMethod")
class VSSETTINGSYSTEM_API UVSSettingItem_AntiAliasingMethod : public UVSConsoleVariableSettingItem
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
	virtual bool EditorAllowChangingItemTag_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingEditorPreviewValue_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetAntiAliasingMethod(EAntiAliasingMethod InMethod);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EAntiAliasingMethod GetAntiAliasingMethod(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
	
private:
#if WITH_EDITORONLY_DATA
	/** Preview window mode that can be modified in editor. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Anti-Aliasing Method", Category = "Settings", Transient)
	TEnumAsByte<EAntiAliasingMethod> EditorPreviewAntiAliasingMethod;
#endif
};
