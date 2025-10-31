// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSObjectFeature.h"
#include "Components/TimelineComponent.h"
#include "VSAlphaBlendFeature.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Gameplay.AlphaBlend")
class VSPLUGINSCORE_API UVSAlphaBlendFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendSignature, UVSAlphaBlendFeature*, Feature,float, Alpha, float, UpdatedTime);

protected:
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;

public:

	/** Get current alpha blend args. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	FAlphaBlendArgs GetAlphaBlendArgs() const { return AlphaBlendArgs; }

	/** Set new alpha blend args and keep current alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (AutoCreateRefTerm = "NewArgs"))
	void SetAlphaBlendArgs(const FAlphaBlendArgs& NewArgs);

	/** Get current alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetAlpha() const { return Alpha; }

	/** Whether alpha blend has finished. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool HasFinished() const;

	/** Enable or disable auto updating. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAutoUpdate(bool bEnabled);

	/** Whether alpha blend is auto updating. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool IsAutoUpdating() const { return bIsAutoUpdating; }

	/** Set time and update alpha accordingly. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetTime(float InTime);

	/** Set alpha and update time accordingly. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAlpha(float InAlpha);

	/** Update alpha blend manually with DeltaTime. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Update(float DeltaTime);

	/** Set playback direction of the alpha blend. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetDirection(ETimelineDirection::Type InDirection);

	/** Reverse current playback direction. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void ReverseDirection();

	/** Set interval between automatic updates. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetUpdateTimeInterval(float Interval = 0.f);

	/** Get current update interval. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetUpdateTimeInterval() const { return UpdateTimeInterval; }

protected:
	/** Execute logic when blend is finished. */
	UFUNCTION(Blueprintable, Category = "AlphaBlend")
	void DoFinishedLogic();
		
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendSignature OnUpdated;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendSignature OnFinished;

public:
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	float DefaultAlpha = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	bool bDefaultAutoUpdate = false;

	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	bool bStopAutoUpdateWhenFinished = true;

protected:
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	TEnumAsByte<ETimelineDirection::Type> Direction = ETimelineDirection::Forward;
	
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	float UpdateTimeInterval = 0.f;
	
private:
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	FAlphaBlendArgs AlphaBlendArgs;

private:
	bool bIsAutoUpdating = false;
	float Alpha = 0.f;
	float UpdatedTime = 0.f;
};
