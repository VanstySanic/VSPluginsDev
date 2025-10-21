// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Generic/VSLayeredActivableWidget.h"
#include "VSGameHUDWidget.generated.h"

class UVSGameWidgetController;
class UVSGameOverlay;
class UCommonAnimatedSwitcher;

/**
 * 
 */
UCLASS()
class VSGENERICPRESETS_API UVSGameHUDWidget : public UVSLayeredActivableWidget
{
	GENERATED_UCLASS_BODY()
	friend class UVSGameWidgetController;

public:
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UVSGameWidgetController* GetGameWidgetController() const { return GameWidgetControllerPrivate.Get(); }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UVSGameOverlay* GetGameOverlay() const { return GameOverlay; }

private:
	void SetOwnerWidgetController(UVSGameWidgetController* InWidgetController);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Widget")
	TObjectPtr<UVSGameOverlay> GameOverlay;

private:
	TWeakObjectPtr<UVSGameWidgetController> GameWidgetControllerPrivate;
};
