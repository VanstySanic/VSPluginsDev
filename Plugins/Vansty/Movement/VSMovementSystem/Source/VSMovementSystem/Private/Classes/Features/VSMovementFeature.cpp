// Copyright VanstySanic. All Rights Reserved.

#include "VSMovementSystem/Public/Classes/Features/VSMovementFeature.h"
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
	return MovementFeatureAgentPrivate.IsValid() ? MovementFeatureAgentPrivate.Get()->MoverComponentPrivate.Get()
		: (GetOwnerActor() ? GetOwnerActor()->FindComponentByClass<UMoverComponent>() : nullptr);
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
	
	Super::Initialize_Implementation();
}

void UVSMovementFeatureAgent::Uninitialize_Implementation()
{
	if (MoverComponentPrivate.IsValid())
	{
		MoverComponentPrivate->InputProducer = OriginalInputProducerPrivate.Get();
	}
	MoverComponentPrivate.Reset();
	
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

// void UVSMovementFeatureAgent::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
// {
// 	/** Produce input for the original input producer first. */
// 	if (OriginalInputProducerPrivate.IsValid() && OriginalInputProducerPrivate->GetClass()->ImplementsInterface(UMoverInputProducerInterface::StaticClass()))
// 	{
// 		Execute_ProduceInput(OriginalInputProducerPrivate.Get(), SimTimeMs, InputCmdResult);
// 	}
//
// 	/** Produce input on all child movement features. */
// }

UObject* UVSMovementFeatureAgent::GetMoverOriginalInputProducer() const
{
	return OriginalInputProducerPrivate.Get();
}