// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputKeySelector.h"
#include "VSInputKeySelector.generated.h"

class SVSInputKeySelector;

/**
 * InputKeySelector extension that adds keyboard/mouse key filtering support.
 */
UCLASS()
class VSWIDGETCORE_API UVSInputKeySelector : public UInputKeySelector
{
	GENERATED_UCLASS_BODY()
	
public:
	//~ Begin UWidget Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void SynchronizeProperties() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void OnWidgetRebuilt() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface
	
public:
	/** Sets whether keyboard keys are allowed in selected key. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetAllowKeyboardKeys(bool bInAllowKeyboardKeys);

	/** Returns true if keyboard keys are allowed, otherwise returns false. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool AllowKeyboardKeys() const;

	/** Sets whether mouse keys are allowed in selected key. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetAllowMouseKeys(bool bInAllowMouseKeys);

	/** Returns true if mouse buttons are allowed, otherwise returns false. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool AllowMouseKeys() const;

private:
	// ReSharper disable once CppHidingFunction
	void HandleIsSelectingKeyChanged();
	
public:
	/** Whether keyboard keys are allowed as selected key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selection")
	uint8 bAllowKeyboardKeys : 1;

	/** Whether mouse keys are allowed as selected key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selection")
	uint8 bAllowMouseKeys : 1;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Key", Category = "Key Selector")
	FInputChord EditorPreviewKey;
#endif
	
	TSharedPtr<SVSInputKeySelector> MyVSInputKeySelector;
};
