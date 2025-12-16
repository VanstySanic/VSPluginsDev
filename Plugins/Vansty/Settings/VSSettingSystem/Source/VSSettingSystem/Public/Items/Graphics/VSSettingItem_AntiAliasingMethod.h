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

public:
	//~ Begin UObject Interface
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;

#if WITH_EDITOR
	virtual bool AllowEditorChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowEditorChangingValueType_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowEditorChangingEditorPreviewValue_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConsoleVariableName_Implementation() const override { return false; }
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
