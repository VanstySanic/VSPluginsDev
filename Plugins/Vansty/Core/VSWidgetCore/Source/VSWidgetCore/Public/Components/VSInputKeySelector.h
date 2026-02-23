// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputKeySelector.h"
#include "VSInputKeySelector.generated.h"

class SVSInputKeySelector;

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSInputKeySelector : public UInputKeySelector
{
	GENERATED_UCLASS_BODY()
	
public:
	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface
	
public:
	/** Sets whether or not keyboard keys are allowed in the selected key. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetAllowKeyboardKeys(bool bInAllowKeyboardKeys);

	/** Returns true if keyboard keys are allowed, otherwise returns false. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool AllowKeyboardKeys() const;

	/** Sets whether mouse buttons are allowed in the selected key. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetAllowMouseKeys(bool bInAllowMouseKeys);

	/** Returns true if mouse buttons are allowed, otherwise returns false. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool AllowMouseKeys() const;

private:
	void HandleIsSelectingKeyChanged();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selection")
	uint8 bAllowKeyboardKeys : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Selection")
	uint8 bAllowMouseKeys : 1;

private:
	TSharedPtr<SVSInputKeySelector> MyVSInputKeySelector;
};
