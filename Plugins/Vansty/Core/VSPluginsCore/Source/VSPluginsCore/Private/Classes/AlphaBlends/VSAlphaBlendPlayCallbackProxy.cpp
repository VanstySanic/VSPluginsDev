// Copyright VanstySanic. All Rights Reserved.


#include "Classes/AlphaBlends/VSAlphaBlendPlayCallbackProxy.h"

UVSAlphaBlendPlayCallBackProxy::UVSAlphaBlendPlayCallBackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSAlphaBlendPlayCallBackProxy* UVSAlphaBlendPlayCallBackProxy::CreateAlphaBlendPlayCallBackProxy(UObject* WorldContext, const FVSAlphaBlendProxyParams& Params)
{
	UVSAlphaBlendPlayCallBackProxy* Node = NewObject<UVSAlphaBlendPlayCallBackProxy>(WorldContext);
	Node->Proxy = UVSAlphaBlendPlayProxy::CreateAlphaBlendPlayProxy(Node, Params);
	
	Node->Proxy->OnAlphaUpdated.AddDynamic(Node, &UVSAlphaBlendPlayCallBackProxy::HandleAlphaUpdated);
	Node->Proxy->OnLoopStart.AddDynamic(Node, &UVSAlphaBlendPlayCallBackProxy::HandleLoopStart);
	Node->Proxy->OnLoopFinished.AddDynamic(Node, &UVSAlphaBlendPlayCallBackProxy::HandleLoopFinished);
	Node->Proxy->OnProxyFinished.AddDynamic(Node, &UVSAlphaBlendPlayCallBackProxy::HandleProxyFinished);
	
	return Node;
}

void UVSAlphaBlendPlayCallBackProxy::Activate()
{
	Super::Activate();

	if (!Proxy)
	{
		SetReadyToDestroy();
	}

	if (UGameInstance* GI = GetWorld() ? GetWorld()->GetWorld()->GetGameInstance() : nullptr)
	{
		RegisterWithGameInstance(GI);
	}
}

void UVSAlphaBlendPlayCallBackProxy::HandleAlphaUpdated(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount)
{
	OnAlphaUpdated.Broadcast(InProxy, Alpha, LoopCount);
}

void UVSAlphaBlendPlayCallBackProxy::HandleLoopStart(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount)
{
	OnLoopStart.Broadcast(InProxy, Alpha, LoopCount);
}

void UVSAlphaBlendPlayCallBackProxy::HandleLoopFinished(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount)
{
	OnLoopFinished.Broadcast(InProxy, Alpha, LoopCount);
}

void UVSAlphaBlendPlayCallBackProxy::HandleProxyFinished(UVSAlphaBlendPlayProxy* InProxy, float Alpha, int32 LoopCount)
{
	OnProxyFinished.Broadcast(InProxy, Alpha, LoopCount);
}
