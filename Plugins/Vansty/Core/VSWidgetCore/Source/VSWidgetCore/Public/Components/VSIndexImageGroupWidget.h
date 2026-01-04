// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetSlotTypes.h"
#include "VSIndexImageGroupWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSIndexImageGroupWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface
	
	UFUNCTION(BlueprintCallable, Category = "Index Image Group")
	void RefreshIndexImages();

	UFUNCTION(BlueprintCallable, Category = "Index Image Group")
	void SetSelectedIndex(int32 Index);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	int32 ImageNum = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	FVSCommonPanelSlotSettings ImageSlotSettings;

	/**
	 * If false, a refreshment of images will be done during pre-construction.
	 * If true, button refreshment should be executed manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	uint8 bDifferRefreshment : 1;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	FSlateBrush SelectedBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index Image Group")
	FSlateBrush UnselectedBrush;
	
protected:
	/** The panel widget that contains the buttons. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Images;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Index Image Group")
	int32 PreviewIndex = INDEX_NONE;
#endif
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UImage>> ImagesPrivate;

	int32 CurrentIndex = 0;
};
