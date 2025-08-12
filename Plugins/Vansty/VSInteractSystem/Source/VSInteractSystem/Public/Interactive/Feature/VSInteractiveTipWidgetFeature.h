// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSInteractiveFeature.h"
#include "VSInteractiveTipWidgetFeature.generated.h"

class UWidgetComponent;
class UWidget;

/**
 * 
 */
UCLASS(DisplayName = "Feature.Interactive.TipWidget")
class VSINTERACTSYSTEM_API UVSInteractiveTipWidgetFeature : public UVSInteractiveFeature
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual bool CanTick_Implementation() const override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void OnFeatureDeactivated_Implementation() override;

private:
	void SetShouldShowTipWidget(bool bShouldShow);
	bool GetShouldShowTipWidget() const { return bShouldShowTipWidget; }
	void ShowTipWidgetImmediately();
	void HideTipWidgetImmediately();
	
	UFUNCTION()
	void OnInspectionStart(UVSInteractFeatureAgent* SourceAgent);

	UFUNCTION()
	void OnInspectionEnd(UVSInteractFeatureAgent* SourceAgent);

	UFUNCTION()
	void OnInteractionStart(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	FName TipWidgetComponentName = FName("TipWidget");

	/**
	 * The time delay to hide the tip widget.
	 * If 0.f, this means no delay.
	 * If below 0.f, delay until manually hide.
	 * Only used when the interaction target is the agent.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	float InteractionHideTipWidgetDelay = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bWidgetOpacityBlendIn = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bWidgetOpacityBlendOut = true;

	/** Any interaction will occur. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bWidgetHideImmediatelyWithInteraction = true;
	
	/** Used to blend the widget opacity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive", meta = (EditCondition = "bWidgetOpacityBlendIn || bWidgetOpacityBlendOut"))
	FAlphaBlendArgs OpacityAlphaBlendArgs;

private:
	TWeakObjectPtr<UWidgetComponent> WidgetComponent;
	TWeakObjectPtr<UWidget> TipWidget;
	TWeakObjectPtr<UVSInteractFeatureAgent> CurrentInteractLocalAgent;

	bool bShouldShowTipWidget = false;
	float WidgetOpacityBlendTime = 0.f;
};
