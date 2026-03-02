// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSSettingItemWidgetMediatorInterface.h"
#include "Mediator/VSWidgetController.h"
#include "UObject/Object.h"
#include "VSSettingItemWidgetController.generated.h"

class UVSSettingItemBase;

/**
 * Widget controller that resolves one setting item by tag and fans updates out to binder mediators.
 *
 * It binds to the setting subsystem at runtime, listens to both "current item" and "any item" updates,
 * and forwards these events to child binders implementing the mediator interface.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemWidgetController : public UVSWidgetController, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	//~ End UObject Interface

protected:
	//~ Begin UVSWidgetController Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	//~ End UVSWidgetController Interface

	//~ Begin IVSSettingItemWidgetMediatorInterface Interface
	virtual UVSSettingItemBase* GetSettingItem_Implementation() const override;
	
	virtual void OnCurrentSettingItemUpdated_Implementation() override;
	virtual void OnAnySettingItemUpdated_Implementation(UVSSettingItemBase* SettingItem) override;
	virtual void EditorRefreshMediator_Implementation() override;
	//~ End IVSSettingItemWidgetMediatorInterface Interface

private:
	void OnCurrentSettingItemUpdatedNative(UVSSettingItemBase* SettingItem);
	
protected:
	/** Target setting item tag resolved through UVSSettingSubsystem. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	FGameplayTag ItemTag;

private:
	TWeakObjectPtr<UVSSettingItemBase> SettingItemPrivate;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TArray<TObjectPtr<UVSWidgetBinder>> LastEditorWidgetBinders;
#endif
};
