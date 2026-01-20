// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "WidgetBinders/VSDesiredBoundWidgetInterface.h"
#include "VSCommonRotator.generated.h"

class UVSIndexImageGroupWidget;

/**
 * CommonUI rotator extension that behaves as a single option selector.
 *
 * This widget wraps UCommonRotator with extra UX features for settings-like
 * option cycling, including optional Prev/Next buttons, navigation wrapping
 * control, and an optional index image group to visualize the current
 * selection state.
 *
 * It can populate default option texts at design/runtime, keeps auxiliary
 * visuals synchronized when the selected item changes, and exposes itself
 * as a desired bind target (typed as "Options") for external binder systems.
 */
UCLASS()
class VSWIDGETCORE_API UVSCommonRotator : public UCommonRotator, public IVSDesiredBoundWidgetInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	virtual bool Initialize() override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	//~ End UUserWidget Interface

	//~ Begin UCommonRotator Interface
	virtual void SetSelectedItem(int32 InValue) override;
	//~ End UCommonRotator Interface
	
	//~ Begin IVSDesiredBoundWidgetInterface Interface
	virtual TMap<UWidget*, FName> GetDesiredBoundTypedWidgets_Implementation() const override;
	//~ End IVSDesiredBoundWidgetInterface Interface
	
private:
	UFUNCTION()
	void OnButtonPrevClicked();

	UFUNCTION()
	void OnButtonNextClicked();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Rotator", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Prev;

	UPROPERTY(BlueprintReadOnly, Category = "Rotator", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Next;

	UPROPERTY(BlueprintReadOnly, Category = "Rotator", meta = (BindWidgetOptional))
	TObjectPtr<UVSIndexImageGroupWidget> IndexImageGroup;

public:
	/** Populate if not empty. */
	UPROPERTY(EditAnywhere, Category = "Rotator")
	TArray<FText> DefaultOptionTexts;
	
	/** If true, allow wrapping in circle when switching options by navigation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotator")
	uint8 bNavigationAllowWrapping : 1;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Rotator")
	int32 EditorPreviewIndex = INDEX_NONE;
#endif
};
