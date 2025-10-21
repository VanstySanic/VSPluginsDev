// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Features/VSObjectFeature.h"
#include "VSGameWidgetController.generated.h"

class UVSNotifyLine;
class UVSGameHUDWidget;
class UVSGameOverlay;
struct FVSWidgetSubtitleParams;
struct FVSWidgetNotifyParams;

/**
 * 
 */
UCLASS(DisplayName = "Feature.WidgetController.Game")
class VSGENERICPRESETS_API UVSGameWidgetController : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void EndPlay_Implementation() override;
	

public:
	UFUNCTION(BlueprintCallable, Category = "Widget")
	APlayerController* GetPlayerController() const { return PlayerControllerPrivate.Get(); }
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
	UVSGameHUDWidget* GetGameHUDWidget() const { return GameHUDWidget; }

	UFUNCTION(BlueprintCallable, Category = "Widget")
	UVSGameOverlay* GetGameOverlay() const;

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "SubtitleParams"))
	void DisplaySubtitleByParams(const FVSWidgetSubtitleParams& SubtitleParams, FName TypeID);

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "SubtitleRow"))
	void DisplaySubtitleByRow(const FDataTableRowHandle& SubtitleRow, FName TypeID);
	
	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "NotifyParams"))
	void DisplayNotifyByParams(const FVSWidgetNotifyParams& NotifyParams, FName TypeID);

	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "NotifyRow"))
	void DisplayNotifyByRow(const FDataTableRowHandle& NotifyRow, FName TypeID);

protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UVSGameHUDWidget> GameHUDWidgetClass;


protected:
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UVSGameHUDWidget> GameHUDWidget;

private:
	TWeakObjectPtr<APlayerController> PlayerControllerPrivate;
};
