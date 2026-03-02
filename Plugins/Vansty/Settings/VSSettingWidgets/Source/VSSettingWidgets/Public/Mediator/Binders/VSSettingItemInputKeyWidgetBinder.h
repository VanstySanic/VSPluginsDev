// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/VSSettingItemWidgetMediatorInterface.h"
#include "Mediator/Binders/VSInputKeyWidgetBinder.h"
#include "VSSettingItemInputKeyWidgetBinder.generated.h"

/**
 * Key-binding adapter that bridges key selector widgets and input-key setting items.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemInputKeyWidgetBinder : public UVSInputKeyWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSInputKeyWidgetBinder Interface
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void OnCurrentSettingItemUpdated_Implementation() override;
	
	virtual TArray<FInputChord> GetExternalKeys_Implementation() const override;
	virtual void OnWidgetKeysUpdated_Implementation(const TArray<FInputChord>& NewKeys) override;
	
#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSInputKeyWidgetBinder Interface

private:
#if WITH_EDITOR
	void RefreshKeySettings();
#endif
	
public:
	/** Slot IDs to read/write from input mapping key maps in UI order. */
	UPROPERTY(EditAnywhere, Category = "Key")
	TArray<int32> KeySlots;
};
