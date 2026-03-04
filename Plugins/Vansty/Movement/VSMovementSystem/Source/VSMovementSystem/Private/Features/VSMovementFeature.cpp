// Copyright VanstySanic. All Rights Reserved.

#include "VSMovementSystem/Public/Features/VSMovementFeature.h"
#include "MoverComponent.h"
#include "Classes/Libraries/VSObjectLibrary.h"

UVSMovementFeature::UVSMovementFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSMovementFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	MovementFeatureAgentPrivate = GetMovementFeatureAgent();
	check(MovementFeatureAgentPrivate.Get());
}

void UVSMovementFeature::Uninitialize_Implementation()
{
	Super::Uninitialize_Implementation();
}

UVSMovementFeatureAgent* UVSMovementFeature::GetMovementFeatureAgent() const
{
	if (MovementFeatureAgentPrivate.IsValid()) return MovementFeatureAgentPrivate.Get();
	if (UVSMovementFeatureAgent* FeatureAgent = FindOwnerFeatureByClass<UVSMovementFeatureAgent>()) return FeatureAgent;
	if (UVSMovementFeatureAgent* FeatureAgent = Cast<UVSMovementFeatureAgent>(const_cast<UVSMovementFeature*>(this))) return FeatureAgent;
	return UVSObjectLibrary::FindFeatureByClassFromObject<UVSMovementFeatureAgent>(GetOwnerActor());
}

UMoverComponent* UVSMovementFeature::GetMoverComponent() const
{
	if (MovementFeatureAgentPrivate.IsValid() && MovementFeatureAgentPrivate->MoverComponentPrivate.IsValid())
		return MovementFeatureAgentPrivate.Get()->MoverComponentPrivate.Get();
	return GetOwnerActor() ? GetOwnerActor()->FindComponentByClass<UMoverComponent>() : nullptr;
}

/** ------------------------------------------------------------------------- **/


UVSMovementFeatureAgent::UVSMovementFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSMovementFeatureAgent::Initialize_Implementation()
{
	MovementFeatureAgentPrivate = this;
	
	MoverComponentPrivate = GetMoverComponent();
	check(MoverComponentPrivate.IsValid());

	if (MoverComponentPrivate.IsValid())
	{
		/** Init the mover component input producer. */
		if (!MoverComponentPrivate->InputProducer)
		{
			if (AActor* ActorOwner = GetOwnerActor())
			{
				if (ActorOwner->GetClass()->ImplementsInterface(UMoverInputProducerInterface::StaticClass()))
				{
					OriginalInputProducerPrivate = ActorOwner;
				}
				else if (UActorComponent* FoundInputProducerComp = ActorOwner->FindComponentByInterface(UMoverInputProducerInterface::StaticClass()))
				{
					OriginalInputProducerPrivate = FoundInputProducerComp;
				}
			}
		}
		else
		{
			OriginalInputProducerPrivate = MoverComponentPrivate->InputProducer;
		}
		
		MoverComponentPrivate->InputProducer = this;
	}
	
	Super::Initialize_Implementation();
}

void UVSMovementFeatureAgent::Uninitialize_Implementation()
{
	if (MoverComponentPrivate.IsValid())
	{
		MoverComponentPrivate->InputProducer = OriginalInputProducerPrivate.Get();
		MoverComponentPrivate.Reset();
	}
	
	Super::Uninitialize_Implementation();
}

void UVSMovementFeatureAgent::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	/** Update the priginal input producer when it is changed in mover. */
	if (MoverComponentPrivate.IsValid() && MoverComponentPrivate->InputProducer != this)
	{
		OriginalInputProducerPrivate = MoverComponentPrivate->InputProducer;
		MoverComponentPrivate->InputProducer = this;
	}
}

void UVSMovementFeatureAgent::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	/** Produce input for the original input producer first. */
	if (OriginalInputProducerPrivate.IsValid() && OriginalInputProducerPrivate.Get() != this
		&& OriginalInputProducerPrivate->GetClass()->ImplementsInterface(UMoverInputProducerInterface::StaticClass()))
	{
		CallProduceInput_NoExecute(OriginalInputProducerPrivate.Get(), SimTimeMs, InputCmdResult);
	}

	for (UVSObjectFeature* SubFeature : GetSubFeatures())
	{
		if (SubFeature && SubFeature->GetClass()->ImplementsInterface(UMoverInputProducerInterface::StaticClass()))
		{
			CallProduceInput_NoExecute(SubFeature, SimTimeMs, InputCmdResult);
		}
	}
}

UObject* UVSMovementFeatureAgent::GetMoverOriginalInputProducer() const
{
	return OriginalInputProducerPrivate.Get();
}

void UVSMovementFeatureAgent::CallProduceInput_NoExecute(UObject* Obj, int32 SimTimeMs, FMoverInputCmdContext& InOutCmd)
{
	if (!Obj) return;
	if (!Obj->GetClass()->ImplementsInterface(UMoverInputProducerInterface::StaticClass())) return;
	
	UFunction* Func = Obj->FindFunction(FName(TEXT("ProduceInput")));
	if (!Func) return;
	
	struct FProduceInput_Params
	{
		int32 SimTimeMs;
		FMoverInputCmdContext InputCmdResult;
	};

	FProduceInput_Params Params;
	Params.SimTimeMs = SimTimeMs;
	Params.InputCmdResult = InOutCmd;

	Obj->ProcessEvent(Func, &Params);

	InOutCmd = MoveTemp(Params.InputCmdResult);
}
