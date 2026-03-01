// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "Mediator/Binders/VSDesiredBoundWidgetInterface.h"
#include "VSCommonRotator.generated.h"

class UVSIndexImageGroupWidget;

/**
 * Rotator extension used as a settings-style option selector.
 *
 * Supports optional Prev/Next buttons, optional no-wrap navigation, and an
 * optional index-image group synchronized with current selection.
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotator", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_Prev;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotator", meta = (BindWidgetOptional))
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
