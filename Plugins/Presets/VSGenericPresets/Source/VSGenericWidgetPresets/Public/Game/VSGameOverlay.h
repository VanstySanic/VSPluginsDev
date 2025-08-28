// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Generic/VSLayeredActivableWidget.h"
#include "VSGameOverlay.generated.h"

struct FVSWidgetNotifyParams;
struct FVSWidgetSubtitleParams;
class UVSSubtitleLine;
class UVSNotifyLine;
class UVerticalBox;


/**
 * 
 */
UCLASS()
class VSGENERICWIDGETPRESETS_API UVSGameOverlay : public UVSLayeredActivableWidget
{
	GENERATED_UCLASS_BODY()
	friend class UVSGameWidgetController;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	TMap<FName, UPanelWidget*> GetTypedSubtitlePanelMap() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	TMap<FName, UPanelWidget*> GetTypedNotifyPanelMap() const;
	
private:
	void SetOwnerWidgetController(UVSGameWidgetController* InWidgetController);
	
	void DisplaySubtitle(const FVSWidgetSubtitleParams& SubtitleParams, FName TypeID);
	void DisplayNotify(const FVSWidgetNotifyParams& NotifyParams, FName TypeID);


protected:
	
	
private:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<FName, TSubclassOf<UVSSubtitleLine>> SubtitleLineClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<FName, TSubclassOf<UVSNotifyLine>> NotifyLineClasses;

private:
	TWeakObjectPtr<UVSGameWidgetController> GameWidgetControllerPrivate;
};
