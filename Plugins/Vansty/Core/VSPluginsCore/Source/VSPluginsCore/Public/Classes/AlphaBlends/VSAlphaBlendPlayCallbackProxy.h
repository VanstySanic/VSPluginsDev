// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSAlphaBlendPlayer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "VSAlphaBlendPlayCallbackProxy.generated.h"

class UVSAlphaBlendPlayProxy;

/**
 * Async Blueprint node wrapper around UVSAlphaBlendPlayProxy lifecycle events.
 */
UCLASS()
class VSPLUGINSCORE_API UVSAlphaBlendPlayCallBackProxy : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAlphaBlendCallBackProxyEvent, UVSAlphaBlendPlayProxy*, Proxy, float, Alpha, int32, LoopCount);

public:
	/** Play an alpha blending process with delegates. */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, DisplayName = "Play Alpha Blend", Category = "AlphaBlend", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Params"))
	static UVSAlphaBlendPlayCallBackProxy* CreateAlphaBlendPlayCallBackProxy(UObject* WorldContext, const FVSAlphaBlendProxyParams& Params);

	//~ Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	//~ End UBlueprintAsyncActionBase Interface

private:
	UFUNCTION()
	void HandleAlphaUpdated(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);

	UFUNCTION()
	void HandleLoopStart(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);

	UFUNCTION()
	void HandleLoopFinished(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);

	UFUNCTION()
	void HandleProxyFinished(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);
	
public:
	/** Broadcast whenever the internal player updates alpha. */
	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnAlphaUpdated;

	/** Broadcast when a loop cycle starts. */
	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnLoopStart;

	/** Broadcast when a loop cycle finishes. */
	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnLoopFinished;

	/** Broadcast once when the proxy reaches final completion. */
	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnProxyFinished;

private:
	/** The underlying proxy object that actually drives the alpha blend. */
	UPROPERTY()
	TObjectPtr<UVSAlphaBlendPlayProxy> Proxy;
};
