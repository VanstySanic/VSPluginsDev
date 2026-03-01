// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSIndexImageGroupWidget.generated.h"

class UImage;

/**
 * Auxiliary widget used to visualize the current index of an external
 * selector, such as a rotator or button group.
 *
 * The widget generates a sequence of images and highlights the currently
 * selected index using configurable brushes, allowing higher-level widgets
 * to drive its state without managing individual image elements.
 */
UCLASS()
class VSWIDGETCORE_API UVSIndexImageGroupWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	/** Rebuilds all index images based on the current ImageNum and layout settings. */
	UFUNCTION(BlueprintCallable, Category = "Index Image Group")
	void RefreshIndexImages();
	
	/** Updates the currently selected index and refreshes image highlight states. */
	UFUNCTION(BlueprintCallable, Category = "Index Image Group")
	void SetSelectedIndex(int32 Index);

public:
	/** Number of images to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	int32 ImageNum = 1;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	FVSCommonPanelSlotSettings ImageSlotSettings;
	
	/**
	 * If false, images are refreshed in pre-construct.
	 * If true, call RefreshIndexImages manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	uint8 bDifferRefreshment : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	FSlateBrush SelectedBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	FSlateBrush UnselectedBrush;
	
protected:
	/** The panel widget that contains generated index images. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Images;
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Index", Category = "Index Image Group")
	int32 EditorPreviewIndex = INDEX_NONE;
#endif
	
	UPROPERTY()
	TArray<TObjectPtr<UImage>> ImagesPrivate;

	int32 CurrentSelectedIndex = 0;
};
