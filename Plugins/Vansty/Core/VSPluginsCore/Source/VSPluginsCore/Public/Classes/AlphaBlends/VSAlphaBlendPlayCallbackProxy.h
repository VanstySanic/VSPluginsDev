// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSAlphaBlendPlayer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "VSAlphaBlendPlayCallbackProxy.generated.h"

class UVSAlphaBlendPlayProxy;

/**
 * UVSAlphaBlendPlayCallBackProxy
 *
 * Blueprint latent-style wrapper for UVSAlphaBlendPlayProxy.
 * Provides a K2 node ("Play Alpha Blend") that runs an alpha-blend sequence
 * using FVSAlphaBlendProxyParams and exposes its events in a Blueprint-friendly
 * async-action format.
 *
 * -------------------------------------------------------------------------
 * Key Features
 * -------------------------------------------------------------------------
 * - Spawns and owns an internal UVSAlphaBlendPlayProxy on Activate().
 * - Forwards all runtime events:
 *     OnProxyCreate
 *     OnAlphaUpdated
 *     OnLoopStart
 *     OnLoopFinished
 *     OnProxyFinished
 * - Can be used directly in Blueprint graphs as an async node.
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * - Call "Play Alpha Blend" with Params to start a sequence.
 * - Bind to the multicast outputs for per-frame or per-loop callbacks.
 * - Automatically cleans up when the internal proxy finishes.
 *
 * -------------------------------------------------------------------------
 * Notes
 * -------------------------------------------------------------------------
 * - This class only handles event forwarding; all blend logic is inside
 *   UVSAlphaBlendPlayProxy.
 * - Use this when you want a single K2 node instead of manually handling
 *   the proxy object in Blueprint.
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

public:
	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnProxyCreate;
	
	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnAlphaUpdated;

	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnLoopStart;

	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnLoopFinished;

	UPROPERTY(BlueprintAssignable)
	FAlphaBlendCallBackProxyEvent OnProxyFinished;

private:
	/** The underlying proxy object that actually drives the alpha blend. */
	UPROPERTY()
	TObjectPtr<UVSAlphaBlendPlayProxy> Proxy;
	
	UFUNCTION()
	void HandleAlphaUpdated(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);

	UFUNCTION()
	void HandleLoopStart(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);

	UFUNCTION()
	void HandleLoopFinished(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);

	UFUNCTION()
	void HandleProxyFinished(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount);
};