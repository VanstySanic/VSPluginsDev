// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "VSSettingWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "VSSettingItemWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingItemWidget : public UUserWidget, public IVSSettingWidgetInterface
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
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	
	virtual void NativeOnHighlighted();
	virtual void NativeOnUnhighlighted();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool IsHighlighted() const { return bIsHighlighted; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	FGameplayTag GetSettingItemSpecifyTag() const { return  SettingItemSpecifyTag; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UWidget* GetCoreWidget() const { return CoreWidget; }
	
protected:
	virtual void BindWidgetToSettingItem_Implementation(const FGameplayTag& SpecifyTag) override;
	virtual void UnbindWidgetFromSettingItem_Implementation(const FGameplayTag& SpecifyTag) override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnHighlighted", Category = "Widget")
	void OnWidgetHighlighted();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnUnhighlighted", Category = "Widget")
	void OnWidgetUnhighlighted();

private:
	void HighlightInternal();
	void UnhighlightInternal();
	void GenerateCoreWidget();

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightSignature OnHighlighted;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FHighlightSignature OnUnhighlighted;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FGameplayTag SettingItemSpecifyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bBindFucusToHighlight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bProcessMouseInTypeOnly = true;
	
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

	bool bIsHighlighted = false;
	bool bMouseInside = false;
	bool bInFocusPath = false;
};
