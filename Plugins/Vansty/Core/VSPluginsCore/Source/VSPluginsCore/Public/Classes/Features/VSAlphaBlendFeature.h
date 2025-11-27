// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/VSObjectFeature.h"
#include "Components/TimelineComponent.h"
#include "VSAlphaBlendFeature.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.Feature.AlphaBlend")
class VSPLUGINSCORE_API UVSAlphaBlendFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendDelegate, UVSAlphaBlendFeature* /** Feature */, float /** Alpha */, float /** CurrentTime */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendEvent, UVSAlphaBlendFeature*, Feature, float, Alpha, float, CurrentTime);

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void EndPlay_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	//~ End UVSObjectFeature Interface.

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

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetTime() const { return UpdatedTime; }
	
	/** Set time and update alpha accordingly. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetTime(float InTime);

	/** Set alpha and update time accordingly. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAlpha(float InAlpha);

	/** Update alpha blend manually with DeltaTime. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Update(float DeltaTime);
	
	/** Reverse current playback direction. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void ReverseDirection();

	/** Set interval between automatic updates. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetUpdateTimeInterval(float Interval = 0.f);

protected:
	/** Execute logic when blend is finished. */
	UFUNCTION(Blueprintable, Category = "AlphaBlend")
	void DoFinishedLogic();
	
public:
	FAlphaBlendDelegate OnUpdated_Native;
	FAlphaBlendDelegate OnFinished_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendEvent OnUpdated;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendEvent OnFinished;
	
public:
	UPROPERTY(EditAnywhere, Category = "AlphaBlend", meta = (ClampMin = "0.f", ClampMax = "1.f"))
	float DefaultAlpha = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	uint8 bDefaultAutoUpdate : 1;

	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	uint8 bStopAutoUpdateWhenFinished : 1;
	
protected:
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	TEnumAsByte<ETimelineDirection::Type> Direction = ETimelineDirection::Forward;

private:
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	FAlphaBlendArgs AlphaBlendArgs;

	uint8 bIsAutoUpdating : 1;
	float Alpha = 0.f;
	float UpdatedTime = 0.f;
};


/** ------------------------------------------------------------------------- **/


USTRUCT()
struct FVSAlphaBlendProxyParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlphaBlendArgs AlphaBlendArgs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoopCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 LoopTimeInterval : 1;
};

UCLASS(BlueprintType)
class UVSAlphaBlendProxy : public UObject
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FAlphaBlendProxyDelegate, UVSAlphaBlendFeature* /** Feature */, float /** Alpha */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAlphaBlendProxyEvent, UVSAlphaBlendFeature*, Feature, float, Alpha);

public:
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (DefaultToSelf = "Outer"))
	static UVSAlphaBlendProxy* CreateAlphaBlendProxy(UObject* Outer);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void StopAtLoopEnd();
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void StopImmediately();
	
public:
	FAlphaBlendProxyDelegate OnUpdated_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnUpdated;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnFinished;

private:
	UPROPERTY()
	TObjectPtr<UVSAlphaBlendFeature> AlphaBlendFeature;

	int32 LoopCount = 1;

};
