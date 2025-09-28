// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "VSSettingWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "Generic/VSHighlightableUserWidget.h"
#include "VSSettingItemWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEMWIDGETS_API UVSSettingItemWidget : public UVSHighlightableUserWidget, public IVSSettingWidgetInterface
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHighlightSignature);

public:
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void NativePreConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	FGameplayTag GetSettingItemSpecifyTag() const { return  SettingItemSpecifyTag; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UWidget* GetCoreWidget() const { return CoreWidget; }
	
protected:
	virtual void BindWidgetToSettingItem_Implementation(const FGameplayTag& SpecifyTag) override;
	virtual void UnbindWidgetFromSettingItem_Implementation(const FGameplayTag& SpecifyTag) override;

private:
	void GenerateCoreWidget();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FGameplayTag SettingItemSpecifyTag;

protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UWidget> CoreWidgetClass;

	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_CoreWidget;

	UPROPERTY()
	TObjectPtr<UWidget> CoreWidget;

#if WITH_EDITOR
	FGameplayTag CachedSettingItemSpecifyTag;
#endif
};
