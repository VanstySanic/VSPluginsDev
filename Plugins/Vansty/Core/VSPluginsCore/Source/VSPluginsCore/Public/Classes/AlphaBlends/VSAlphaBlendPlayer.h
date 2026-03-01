// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/VSTickableObject.h"
#include "Components/TimelineComponent.h"
#include "VSAlphaBlendPlayer.generated.h"

/**
 * Tickable wrapper around FAlphaBlend with direction control and auto-update support.
 */
UENUM(BlueprintType)
namespace EVSAlphaBlendFinishAction
{
	enum Type
	{
		/** Keep current state after reaching an end. */
		None,
		/** Snap to the opposite end once finished (0 <-> 1). */
		JumpToAnotherPole,
		/** Reverse playback direction and continue. */
		ReverseDirection,
		/** Disable Tick-driven updates after finish. */
		StopAutoUpdating,
		/** Destroy the player object after finish. */
		Destroy,
	};	
}

/**
 * Tickable alpha-blend player with direction control and optional auto update.
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
	/** Initializes runtime state from defaults. Safe to call repeatedly. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	FAlphaBlendArgs GetAlphaBlendArgs() const { return AlphaBlendArgs; }

	/** Replaces blend args while preserving the current normalized alpha. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (AutoCreateRefTerm = "NewArgs"))
	void SetAlphaBlendArgs(const FAlphaBlendArgs& NewArgs);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetBlendTime(float BlendTime);
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetAlpha() const { return CurrentAlpha; }

	/** Returns true when current direction has reached its terminal end. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool HasFinished() const;

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAutoUpdate(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool IsAutoUpdating() const { return bIsAutoUpdating; }
	
	/** Sets alpha directly and synchronizes internal playback time. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetAlpha(float InAlpha);

	/** Advances alpha by DeltaTime using current direction and blend args. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Update(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetDirection(ETimelineDirection::Type Direction);
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void ReverseDirection();

	/**
	 * Reset the alpha blend process from the default values.
	 * @param bKeepAutoUpdatingState Whether to keep the current auto updating state.
	 */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Reset(bool bKeepAutoUpdatingState = true);

	/** Explicitly destroys this runtime player object. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Destroy();

private:
	float GetTimeByAlpha(float InAlpha);
	
	/** Sets playback time and recomputes alpha from blend settings. */
	void SetTime(float InTime);
	
	/** Execute logic when blend is finished. */
	void FinishInternal();
	
public:
	/** Native event fired whenever alpha changes. */
	FAlphaBlendDelegate OnUpdated_Native;
	/** Native event fired when current direction reaches its terminal edge. */
	FAlphaBlendDelegate OnFinished_Native;
	
	/** Blueprint mirror of OnUpdated_Native. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendEvent OnUpdated;

	/** Blueprint mirror of OnFinished_Native. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendEvent OnFinished;
	
protected:
	/** Blend curve and timing configuration. */
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	FAlphaBlendArgs AlphaBlendArgs;

public:
	/** Default direction applied during Initialize. */
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	TEnumAsByte<ETimelineDirection::Type> DefaultDirection = ETimelineDirection::Forward;
	
	/** Default alpha applied during Initialize (clamped to [0,1]). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AlphaBlend", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DefaultAlpha = 0.f;
		
	/** Default Tick-driven update state applied during Initialize. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AlphaBlend")
	uint8 bDefaultAutoUpdate : 1;
	
	/** Ordered actions executed when FinishInternal is reached. */
	UPROPERTY(EditAnywhere, Category = "AlphaBlend")
	TArray<TEnumAsByte<EVSAlphaBlendFinishAction::Type>> FinishActions;

private:
	uint8 bHasBeenInitialized : 1;
	ETimelineDirection::Type CurrentDirection = ETimelineDirection::Forward;
	/** Normalized alpha; < 0 means uninitialized. */
	float CurrentAlpha = -1.f;
	/** Playback time in seconds; < 0 means uninitialized. */
	float CurrentTime = -1.f;
	uint8 bIsAutoUpdating : 1;
};


/** ------------------------------------------------------------------------- **/


/**
 * Runtime parameters used to initialize and run UVSAlphaBlendPlayProxy.
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

	/** Blend curve and timing used by the internal player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlphaBlendArgs AlphaBlendArgs;
	
	/** Direction used when each loop starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETimelineDirection::Type> StartDirection = ETimelineDirection::Forward;
	
	/** Number of loops to run. <= 0 means infinite looping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoopCount = 0;
	
	/** Initial alpha used before first update. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartAlpha = 0.f;
	
	/** Delay before first loop starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialDelay = 0.f;

	/** Delay between completed loops. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoopTimeInterval = 0.f;

	/** Fixed update step. 0 means update every tick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpdateTimeInterval = 0.f;

	/** Delay before reversing direction in ping-pong mode. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bLoopRequiresReversingDirection"))
	float DirectionReverseTimeInterval = 0.0;
	
	/** If true, a full cycle will include an update reversed from the start direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bLoopRequiresReversingDirection : 1;

	/** If true, starts the cycle in already-reversed state for ping-pong loops. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bLoopRequiresReversingDirection"))
	uint8 bLoopDirectionAlreadyReversedAtStart : 1;

	/** Pause proxy when all requested loops complete. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bPauseWhenFinished : 1;
	
	/** Destroy proxy automatically when all requested loops complete. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bDestroyWhenFinished : 1;

	/** Start paused and wait for an explicit SetPaused(false). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bStartPaused : 1;

	/** If true, proxy ticks while the world is paused. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bUpdateWhenGamePaused : 1;
};

/**
 * Proxy that plays one or more alpha-blend cycles with delay/loop/pause support.
 */
UCLASS(BlueprintType)
class VSPLUGINSCORE_API UVSAlphaBlendPlayProxy : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendProxyDelegate, UVSAlphaBlendPlayProxy* /** Proxy */, float /** Alpha */, int32 /** CurrentLoop */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendProxyEvent, UVSAlphaBlendPlayProxy*, Proxy, float, Alpha, int32, CurrentLoop);

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual bool CanExecuteTick_Implementation() const override;
	//~ End UVSObjectFeature Interface.
	
public:
	/** Factory that creates and optionally defers initialization of the proxy. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Params"))
	static UVSAlphaBlendPlayProxy* CreateAlphaBlendPlayProxy(UObject* WorldContext, const FVSAlphaBlendProxyParams& Params, bool bDifferInitialization = false);
	
	/** Initializes runtime state from cached proxy params. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Initialize();
	
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	float GetAlpha() const;

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	int32 GetCurrentLoop() const { return CurrentLoop; }

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	FVSAlphaBlendProxyParams GetProxyParams() const { return CachedProxyParams; }

	/** Returns true when proxy has completed all loops and finish logic. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool HasFinished() const { return bHasFinished; }

	/** Resets loop/delay/player state to initial values. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Reset(bool bIgnoreInitialDelay = false, bool bKeepPauseState = true);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void SetPaused(bool bPaused);

	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	bool IsPaused() const { return bIsPaused; }

	/** Destroys the proxy and releases its internal player. */
	UFUNCTION(BlueprintCallable, Category = "AlphaBlend")
	void Destroy();

private:
	ETimelineDirection::Type CalcDefaultDirection() const;
	void AlphaBlendPlayerFinished();
	void CheckNextLoop();
	void DoFinishLogics();
	
public:
	/** Broadcast whenever the internal player updates alpha. */
	FAlphaBlendProxyDelegate OnAlphaUpdated_Native;
	/** Broadcast when a loop cycle starts. */
	FAlphaBlendProxyDelegate OnLoopStart_Native;
	/** Broadcast when a loop cycle finishes. */
	FAlphaBlendProxyDelegate OnLoopFinished_Native;
	/** Broadcast once when the proxy reaches final completion. */
	FAlphaBlendProxyDelegate OnProxyFinished_Native;

	/** Broadcast whenever the internal player updates alpha. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnAlphaUpdated;
	
	/** Broadcast when a loop cycle starts. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnLoopStart;
	
	/** Broadcast when a loop cycle finishes. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnLoopFinished;
	
	/** Broadcast once when the proxy reaches final completion. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FAlphaBlendProxyEvent OnProxyFinished;

private:
	/** Internal player that evaluates and updates alpha over time. */
	UPROPERTY()
	TObjectPtr<UVSAlphaBlendPlayer> AlphaBlendPlayer;
	
	/** Immutable copy of create-time configuration used for resets/re-init. */
	FVSAlphaBlendProxyParams CachedProxyParams;
	uint8 bHasBeenInitialized : 1;
	uint8 bIsPaused : 1;
	uint8 bHasFinished : 1;
	/** 1-based loop counter after first loop starts. */
	int32 CurrentLoop = 0;
	/** Seconds; < 0 means delay not initialized. */
	float RemainedInitialDelay = -1.f;
	/** Seconds; < 0 means interval delay not initialized. */
	float RemainedLoopTimeInterval = -1.f;
	/** Seconds; < 0 means reverse delay not initialized. */
	float RemainedDirectionTimeInterval = -1.f;
};
