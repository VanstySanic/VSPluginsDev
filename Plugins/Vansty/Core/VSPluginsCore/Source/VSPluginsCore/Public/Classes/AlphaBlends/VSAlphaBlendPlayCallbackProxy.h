// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSAlphaBlendPlayer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "VSAlphaBlendPlayCallbackProxy.generated.h"

class UVSAlphaBlendPlayProxy;

/**
 * Blueprint async-action class that runs an alpha-blend sequence using
 * UVSAlphaBlendPlayProxy and exposes its lifecycle events to Blueprint.
 *
 * This class is intended to be used as a single K2 node, handling proxy
 * creation, activation, and event forwarding, without requiring manual
 * proxy management in Blueprint graphs.
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