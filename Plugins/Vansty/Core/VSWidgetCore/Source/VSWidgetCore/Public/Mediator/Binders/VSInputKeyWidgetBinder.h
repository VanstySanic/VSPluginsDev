// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders/VSWidgetBinder.h"
#include "Types/VSWidgetSettingTypes.h"
#include "VSInputKeyWidgetBinder.generated.h"

class UVSContentedInputKeySelector;
class UVSInputKeySelectorGroupWidget;
class UVSKeyIconConfig;

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSInputKeyWidgetBinder : public UVSWidgetBinder
{
	GENERATED_UCLASS_BODY()
	
protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	//~ End UVSWidgetBinder Interface

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Key")
	TArray<FInputChord> GetExternalKeys() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Key")
	TArray<FInputChord> GetWidgetKeys() const;

	UFUNCTION(BlueprintCallable, Category = "Key")
	FInputChord GetExternalKeyAtIndex(int32 Index = 0);

	UFUNCTION(BlueprintCallable, Category = "Key")
	FInputChord GetWidgetKeyAtIndex(int32 Index = 0) const;
	
	UFUNCTION(BlueprintCallable, Category = "Key Selector Group")
	void RefreshKeys();
	
protected:
	void OnBoundWidgetKeysUpdated();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Key")
	void OnWidgetKeysUpdated(const TArray<FInputChord>& NewKeys);
	
#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingKeySettings() const;
#endif
private:
	void OnInputKeyGroupUpdated(UVSInputKeySelectorGroupWidget* Group);
	void OnContentedSelectorKeySelected(UVSContentedInputKeySelector* Selector, FInputChord SelectedKey);

	UFUNCTION()
	void OnInputKeySelectorKeySelected(FInputChord SelectedKey);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Key", meta = (EditCondition = "EditorAllowChangingKeySettings()"))
	FVSInputKeySelectorKeySettings KeySettings;
};
