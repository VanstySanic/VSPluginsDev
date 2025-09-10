// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Groups/CommonWidgetGroupBase.h"
#include "VSImageIndexWidgetGroup.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSImageIndexWidgetGroup : public UCommonWidgetGroupBase
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UWidget> GetWidgetType() const override;

	UVSImageIndexWidgetGroup();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetIndex(int32 InIndex);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	int32 GetIndex() const { return Index; }

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "InBrush"))
	void SetIndexedBrush(const FSlateBrush& InBrush);

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "InBrush"))
	void SetUnindexedBrush(const FSlateBrush& InBrush);
	
protected:
	virtual void OnWidgetAdded(UWidget* NewWidget) override;
	virtual void OnWidgetRemoved(UWidget* OldWidget) override;
	virtual void OnRemoveAll() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FSlateBrush IndexedBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FSlateBrush UnindexedBrush;
	
private:
	TArray<TWeakObjectPtr<UImage>> Images;
	int32 Index = INDEX_NONE;
};
