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
 * 
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
	UPROPERTY(EditAnywhere, Category = "Settings")
	FGameplayTag ItemTag;

private:
	TWeakObjectPtr<UVSSettingItemBase> SettingItemPrivate;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TArray<TObjectPtr<UVSWidgetBinder>> LastEditorWidgetBinders;
#endif
};
