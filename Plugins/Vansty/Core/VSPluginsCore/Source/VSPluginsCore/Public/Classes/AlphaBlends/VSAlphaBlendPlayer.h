// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/VSTickableObject.h"
#include "Components/TimelineComponent.h"
#include "VSAlphaBlendPlayer.generated.h"

/** Tickable wrapper around FAlphaBlend with direction control and auto-update support. */
UENUM(BlueprintType)
namespace EVSAlphaBlendFinishAction
{
	enum Type
	{
		None,
		JumpToAnotherPole,
		ReverseDirection,
		StopAutoUpdating,
		Destroy,
	};	
}

/**
 * Parameter set describing how an alpha-blend play proxy executes.
 *
 * Defines timing, looping behavior, direction rules, pause semantics,
 * and lifecycle options used to configure a proxy-driven blend sequence.
 */
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class VSPLUGINSCORE_API UVSAlphaBlendPlayer : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FAlphaBlendDelegate, UVSAlphaBlendPlayer* /** Player */, float /** Alpha */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAlphaBlendEvent, UVSAlphaBlendPlayer*, Player, float, Alpha);

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Tick_Implementation(float DeltaTime) override;
	//~ End UVSObjectFeature Interface.

public:
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Initialize();

	/** Get current alpha blend args. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	FAlphaBlendArgs GetAlphaBlendArgs() const { return AlphaBlendArgs; }

	/** Set new alpha blend args and keep current alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (AutoCreateRefTerm = "NewArgs"))
	void SetAlphaBlendArgs(const FAlphaBlendArgs& NewArgs);

	/** Set the blend time in args. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetBlendTime(float BlendTime);
	
	/** Get current alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetAlpha() const { return CurrentAlpha; }

	/** Whether alpha blend has finished. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool HasFinished() const;

	/** Enable or disable auto updating. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAutoUpdate(bool bEnabled);

	/** Whether alpha blend is auto updating. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool IsAutoUpdating() const { return bIsAutoUpdating; }
	
	/** Set alpha and update time accordingly. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAlpha(float InAlpha);

	/** Update alpha blend manually with DeltaTime. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Update(float DeltaTime);

	/** Set whether to update forward or backward. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetDirection(ETimelineDirection::Type Direction);
	
	/** Reverse current playback direction. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void ReverseDirection();

	/**
	 * Reset the alpha blend process from the default values.
	 * @param bKeepAutoUpdatingState Whether to keep the current auto updating state.
	 */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Reset(bool bKeepAutoUpdatingState = true);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Destroy();

private:
	float GetTimeByAlpha(float InAlpha);
	
	/** Set time and update alpha accordingly. */
	void SetTime(float InTime);
	
	/** Execute logic when blend is finished. */
	void FinishInternal();
	
public:
	FAlphaBlendDelegate OnUpdated_Native;
	FAlphaBlendDelegate OnFinished_Native;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendEvent OnUpdated;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendEvent OnFinished;
	
protected:
	/** Defines the blend time and curves. */
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	FAlphaBlendArgs AlphaBlendArgs;

public:
	/** The timeline direction to define forward or backward updating behavior. */
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	TEnumAsByte<ETimelineDirection::Type> DefaultDirection = ETimelineDirection::Forward;
	
	/** The default alpha to apply during initialization. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AlphaBlend", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DefaultAlpha = 0.f;
		
	/** Whether to enable auto update during initialization. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AlphaBlend")
	uint8 bDefaultAutoUpdate : 1;
	
	/** What to do when the blend finishes. */
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	TArray<TEnumAsByte<EVSAlphaBlendFinishAction::Type>> FinishActions;

private:
	uint8 bHasBeenInitialized : 1;
	ETimelineDirection::Type CurrentDirection = ETimelineDirection::Forward;
	float CurrentAlpha = -1.f;
	float CurrentTime = -1.f;
	uint8 bIsAutoUpdating : 1;
};


/** ------------------------------------------------------------------------- **/


/**
 * High-level controller for executing alpha-blend sequences.
 *
 * Manages one or more blend cycles using an internal alpha-blend player,
 * handling delays, looping, optional direction reversal, pause control,
 * and lifecycle completion, with Blueprint-friendly events.
 */
USTRUCT(BlueprintType)
struct FVSAlphaBlendProxyParams
{
	GENERATED_USTRUCT_BODY()

	FVSAlphaBlendProxyParams()
		: bLoopRequiresReversingDirection(false)
		, bLoopDirectionAlreadyReversedAtStart(false)
		, bPauseWhenFinished(true)
		, bDestroyWhenFinished(true)
		, bStartPaused(false)
		, bUpdateWhenGamePaused(false)
	{
		
	}

	/** Defines the numerical alpha blending performance with time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlphaBlendArgs AlphaBlendArgs;
	
	/** The default direction to set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETimelineDirection::Type> StartDirection = ETimelineDirection::Forward;
	
	/** The count to perform loops. If <= 0, the loops will last forever. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoopCount = 0;
	
	/** The default alpha to set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartAlpha = 0.f;
	
	/** The time to delay before the blending starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialDelay = 0.f;

	/** The time interval to delay the start for the next cycle when the previous cycle finishes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoopTimeInterval = 0.f;

	/** Used to optimize performance. If 0.f, update every frame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpdateTimeInterval = 0.f;

	/** The directional time delay before the direction reverse. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bLoopRequiresReversingDirection"))
	float DirectionReverseTimeInterval = 0.0;
	
	/** If true, a full cycle will include an update reversed from the start direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bLoopRequiresReversingDirection : 1;

	/** Whether to indicate that the direction has already been reversed at start, if the loop cycle requires direction reversing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bLoopRequiresReversingDirection"))
	uint8 bLoopDirectionAlreadyReversedAtStart : 1;

	/** Whether to pause when the proxy finishes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bPauseWhenFinished : 1;
	
	/** Whether to destroy the proxy when it finishes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bDestroyWhenFinished : 1;

	/** Whether to pause when the proxy is created. If true, user should toggle the pause state manually. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bStartPaused : 1;

	/** Whether to update when the game is paused. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bUpdateWhenGamePaused : 1;
};

/**
 * UVSAlphaBlendPlayProxy
 *
 * High-level controller that executes one or more alpha-blend cycles using a
 * UVSAlphaBlendPlayer instance, with optional delays, loop logic, and pause
 * control. Designed for Blueprint-friendly sequence playback.
 *
 * -------------------------------------------------------------------------
 * Key Features
 * -------------------------------------------------------------------------
 * - Handles initial delay, per-loop delay, and optional direction reversing.
 * - Supports pausing, resetting, and update-interval throttling.
 * - Tracks loop index and completion state.
 *
 * - Events:
 *   - OnAlphaUpdated
 *   - OnLoopStart / OnLoopFinished
 *   - OnProxyFinished
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * - Create via CreateAlphaBlendPlayProxy(WorldContext, Params).
 * - Bind to events for per-frame and per-cycle notifications.
 * - Use SetPaused(), Reset(), Destroy() when needed.
 */
UCLASS(BlueprintType)
class VSPLUGINSCORE_API UVSAlphaBlendPlayProxy : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendProxyDelegate, UVSAlphaBlendPlayProxy* /** Proxy */, float /** Alpha */, int32 /** LoopCount */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendProxyEvent, UVSAlphaBlendPlayProxy*, Proxy, float, Alpha, int32, LoopCount);

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual bool CanExecuteTick_Implementation() const override;
	//~ End UVSObjectFeature Interface.
	
public:
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Params"))
	static UVSAlphaBlendPlayProxy* CreateAlphaBlendPlayProxy(UObject* WorldContext, const FVSAlphaBlendProxyParams& Params, bool bDifferInitialization = false);
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Initialize();
	
	/** Get the current updated alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetAlpha() const;

	/** Get the current loop count index. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	int32 GetLoopCount() const { return CurrentLoopCount; }

	/** Get the params that defines the alpha blending. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	FVSAlphaBlendProxyParams GetProxyParams() const { return CachedProxyParams; }

	/** Is the alpha blending totally finished. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool HasFinished() const { return bHasFinished; }

	/** Reset the proxy to the initial states. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Reset(bool bIgnoreInitialDelay = false, bool bKeepPauseState = true);

	/** Set whether to pause the update. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetPaused(bool bPaused);

	/** Whether the update is paused. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool IsPaused() const { return bIsPaused; }

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Destroy();

private:
	ETimelineDirection::Type CalcDefaultDirection() const;
	void AlphaBlendPlayerFinished();
	void CheckNextLoop();
	void DoFinishLogics();
	
public:
	FAlphaBlendProxyDelegate OnAlphaUpdated_Native;
	FAlphaBlendProxyDelegate OnLoopStart_Native;
	FAlphaBlendProxyDelegate OnLoopFinished_Native;
	FAlphaBlendProxyDelegate OnProxyFinished_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnAlphaUpdated;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnLoopStart;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnLoopFinished;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnProxyFinished;

private:
	UPROPERTY()
	TObjectPtr<UVSAlphaBlendPlayer> AlphaBlendPlayer;
	
	FVSAlphaBlendProxyParams CachedProxyParams;
	uint8 bHasBeenInitialized : 1;
	uint8 bIsPaused : 1;
	uint8 bHasFinished : 1;
	int32 CurrentLoopCount = 0;
	float RemainedInitialDelay = -1.f;
	float RemainedLoopTimeInterval = -1.f;
	float RemainedDirectionTimeInterval = -1.f;
};

