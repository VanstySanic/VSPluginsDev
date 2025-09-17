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

public:
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	FAlphaBlendArgs GetAlphaBlendArgs() const { return AlphaBlendArgs; }

	/** Set the alpha blend and keep current alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (AutoCreateRefTerm = "NewArgs"))
	void SetAlphaBlendArgs(const FAlphaBlendArgs& NewArgs);
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetAlpha() const { return Alpha; }

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool HasFinished() const;
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAutoUpdate(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool IsAutoUpdating() const { return bIsAutoUpdating; }

	/** Set alpha and update the alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetTime(float InTime);

	/** Set alpha and update the time. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAlpha(float InAlpha);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Update(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetDirection(ETimelineDirection::Type InDirection);
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void ReverseDirection();
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetUpdateTimeInterval(float Interval = 0.f);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetUpdateTimeInterval() const { return UpdateTimeInterval; }

protected:
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
