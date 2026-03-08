// Copyright VanstySanic. All Rights Reserved.

#include "VSMoverSystem/Public/Features/VSMoverFeature.h"
#include "MoverComponent.h"
#include "VSMoverSystemUtils.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSObjectLibrary.h"

UVSMoverFeature::UVSMoverFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSMoverFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	MoverFeatureAgentPrivate = GetMoverFeatureAgent();
	check(MoverFeatureAgentPrivate.IsValid());
}

void UVSMoverFeature::Uninitialize_Implementation()
{
	MoverFeatureAgentPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

UVSMoverFeatureAgent* UVSMoverFeature::GetMoverFeatureAgent() const
{
	if (MoverFeatureAgentPrivate.IsValid()) return MoverFeatureAgentPrivate.Get();
	if (UVSMoverFeatureAgent* FeatureAgent = Cast<UVSMoverFeatureAgent>(const_cast<UVSMoverFeature*>(this))) return FeatureAgent;
	if (UVSMoverFeatureAgent* FeatureAgent = FindOwnerFeatureByClass<UVSMoverFeatureAgent>()) return FeatureAgent;
	return UVSObjectLibrary::FindFeatureByClassFromObject<UVSMoverFeatureAgent>(GetOwnerActor());
}

UMoverComponent* UVSMoverFeature::GetMoverComponent() const
{
	if (MoverFeatureAgentPrivate.IsValid() && MoverFeatureAgentPrivate->MoverComponentPrivate.IsValid())
	{
		return MoverFeatureAgentPrivate.Get()->MoverComponentPrivate.Get();
	}
	
	return GetOwnerActor() ? GetOwnerActor()->FindComponentByClass<UMoverComponent>() : nullptr;
}

FVector UVSMoverFeature::GetAngularVelocity_Implementation() const
{
	return MoverFeatureAgentPrivate.IsValid() ? MoverFeatureAgentPrivate->RealAngularVelocity : FVector::ZeroVector;
}

FVector UVSMoverFeature::GetAcceleration_Implementation() const
{
	return MoverFeatureAgentPrivate.IsValid() ? MoverFeatureAgentPrivate->RealAcceleration : FVector::ZeroVector;
}

FVector UVSMoverFeature::GetMovementInput_Implementation() const
{
	if (UMoverComponent* MoverComponent = GetMoverComponent())
	{
		if (MoverComponent->HasValidCachedInputCmd())
		{
			if (const FCharacterDefaultInputs* CharacterInputs = MoverComponent->GetLastInputCmd().InputCollection.FindMutableDataByType<FCharacterDefaultInputs>())
			{
				if (CharacterInputs->GetMoveInputType() == EMoveInputType::DirectionalIntent)
				{
					return CharacterInputs->GetMoveInput_WorldSpace();
				}
			}
		}
	}

	return Super::GetMovementInput_Implementation();
}

USceneComponent* UVSMoverFeature::GetUpdatedComponent_Implementation() const
{
	if (UMoverComponent* MoverComponent = GetMoverComponent())
	{
		return MoverComponent->GetUpdatedComponent();
	}
	
	return Super::GetUpdatedComponent_Implementation();
}

UPrimitiveComponent* UVSMoverFeature::GetMovementBase_Implementation() const
{
	return GetMoverComponent() ? GetMoverComponent()->GetMovementBase() : nullptr;
}

FName UVSMoverFeature::GetMovementBaseBoneName_Implementation() const
{
	return GetMoverComponent() ? GetMoverComponent()->GetMovementBaseBoneName() : NAME_None;
}

FGameplayTag UVSMoverFeature::GetMovementMode_Implementation() const
{
	if (UMoverComponent* MoverComponent = GetMoverComponent())
	{
		return UVSMoverSystemUtils::GetUnnamedMovementMode(MoverComponent->GetMovementModeName());
	}
	
	return Super::GetMovementMode_Implementation();
}

void UVSMoverFeature::UpdateGeneratedMovement_Implementation(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove)
{
}

/** ------------------------------------------------------------------------- **/


UVSMoverFeatureAgent::UVSMoverFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSMoverFeatureAgent::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	MoverComponentPrivate = GetMoverComponent();
	check(MoverComponentPrivate.IsValid());

	if (MoverComponentPrivate.IsValid())
	{
		/** Init the mover component input producer and save the original one. */
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

		if (MoverComponentPrivate->ProcessGeneratedMovement.IsBound())
		{
			OriginalProcessGeneratedMovement = MakeShared<FMover_ProcessGeneratedMovement>(MoverComponentPrivate->ProcessGeneratedMovement);
		}
		MoverComponentPrivate->ProcessGeneratedMovement.BindDynamic(this, &UVSMoverFeatureAgent::OnMoverGenerateMovement);
		ThisProcessGeneratedMovement = MakeShared<FMover_ProcessGeneratedMovement>(MoverComponentPrivate->ProcessGeneratedMovement);

		MoverComponentPrivate->OnMovementModeChanged.AddDynamic(this, &UVSMoverFeatureAgent::OnMoverMovementModeChanged);

		LastUpdateVelocity = MoverComponentPrivate->GetVelocity();
		LastUpdateRotation = GetOwnerActor()->GetActorRotation();
	}
	
	if (UVSGameplayTagFeatureBase* GameplayTagFeature = GetPrimaryGameplayTagFeature_Native())
	{
		GameplayTagFeature->BindDelegateForObject(this);
	}
}

void UVSMoverFeatureAgent::BeginPlay_Implementation()
{
	/** Sync states. */
	if (MoverComponentPrivate.IsValid() && MoverComponentPrivate->HasBegunPlay())
	{
		OnMoverMovementModeChanged(NAME_None, MoverComponentPrivate->GetMovementModeName());
	}
	
	Super::BeginPlay_Implementation();
}

void UVSMoverFeatureAgent::Uninitialize_Implementation()
{
	if (UVSGameplayTagFeatureBase* GameplayTagFeature = GetPrimaryGameplayTagFeature_Native())
	{
		GameplayTagFeature->UnbindDelegateForObject(this);
	}
	
	if (MoverComponentPrivate.IsValid())
	{
		if (OriginalProcessGeneratedMovement.IsValid())
		{
			MoverComponentPrivate->ProcessGeneratedMovement = *OriginalProcessGeneratedMovement;
		}
		else
		{
			MoverComponentPrivate->ProcessGeneratedMovement.Unbind();
		}
		MoverComponentPrivate->InputProducer = OriginalInputProducerPrivate.Get();
		MoverComponentPrivate->OnMovementModeChanged.RemoveDynamic(this, &UVSMoverFeatureAgent::OnMoverMovementModeChanged);
	}
	
	OriginalInputProducerPrivate.Reset();
	OriginalProcessGeneratedMovement.Reset();
	ThisProcessGeneratedMovement.Reset();
	MoverComponentPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

void UVSMoverFeatureAgent::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	/** Update the original input producer when it is changed in mover. */
	if (MoverComponentPrivate.IsValid() && MoverComponentPrivate->InputProducer != this)
	{
		OriginalInputProducerPrivate = MoverComponentPrivate->InputProducer;
		MoverComponentPrivate->InputProducer = this;
	}
	
	/** Update the original movement post generator. */
	if (MoverComponentPrivate.IsValid() && ThisProcessGeneratedMovement.IsValid())
	{
		if (MoverComponentPrivate->ProcessGeneratedMovement != *ThisProcessGeneratedMovement)
		{
			OriginalProcessGeneratedMovement = MakeShared<FMover_ProcessGeneratedMovement>(MoverComponentPrivate->ProcessGeneratedMovement);
			MoverComponentPrivate->ProcessGeneratedMovement.BindDynamic(this, &UVSMoverFeatureAgent::OnMoverGenerateMovement);
			ThisProcessGeneratedMovement = MakeShared<FMover_ProcessGeneratedMovement>(MoverComponentPrivate->ProcessGeneratedMovement);
		}
	}
}

void UVSMoverFeatureAgent::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	Super::OnGameplayTagFeatureTagsUpdated_Implementation(GameplayTagFeature);

	for (UVSObjectFeature* Feature : GetSubFeatures())
	{
		if (Feature && Feature->IsA<UVSMoverFeature>() && Feature->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
		{
			Execute_OnGameplayTagFeatureTagsUpdated(Feature, GameplayTagFeature);
		}
	}
}

void UVSMoverFeatureAgent::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	Super::OnGameplayTagFeatureTagEventsNotified_Implementation(GameplayTagFeature, TagEvents);

	for (UVSObjectFeature* Feature : GetSubFeatures())
	{
		if (Feature && Feature->IsA<UVSMoverFeature>() && Feature->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
		{
			Execute_OnGameplayTagFeatureTagEventsNotified(Feature, GameplayTagFeature, TagEvents);
		}
	}
}

void UVSMoverFeatureAgent::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
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

void UVSMoverFeatureAgent::CallProduceInput_NoExecute(UObject* Obj, int32 SimTimeMs, FMoverInputCmdContext& InOutCmd)
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

bool UVSMoverFeatureAgent::CanUpdateMovement_Implementation() const
{
	return Super::CanUpdateMovement_Implementation() && MoverComponentPrivate.IsValid();
}

void UVSMoverFeatureAgent::UpdateMovement_Implementation(float DeltaTime)
{
	Super::UpdateMovement_Implementation(DeltaTime);

	const FVector VelocityDelta = MoverComponentPrivate->GetVelocity() - LastUpdateVelocity;
	const FRotator AngularDelta = (GetOwnerActor()->GetActorRotation() - LastUpdateRotation).GetNormalized();
	
	RealAcceleration = VelocityDelta / DeltaTime;
	RealAngularVelocity = FVector(AngularDelta.Roll, AngularDelta.Pitch, AngularDelta.Yaw) / DeltaTime;

	LastUpdateVelocity = MoverComponentPrivate->GetVelocity();
	LastUpdateRotation = GetOwnerActor()->GetActorRotation();
}

void UVSMoverFeatureAgent::OnMoverGenerateMovement(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove)
{
	/** Process the original movement generator first. */
	if (OriginalProcessGeneratedMovement.IsValid())
	{
		OriginalProcessGeneratedMovement->ExecuteIfBound(StartState, TimeStep, OutProposedMove);
	}

	if (CanUpdateMovement())
	{
		UpdateGeneratedMovement(StartState, TimeStep, OutProposedMove);
	}

	for (UVSObjectFeature* SubFeature : GetSubFeatures())
	{
		if (UVSMoverFeature* MoverFeature = Cast<UVSMoverFeature>(SubFeature))
		{
			if (MoverFeature->CanUpdateMovement())
			{
				MoverFeature->UpdateGeneratedMovement(StartState, TimeStep, OutProposedMove);
			}
		}
	}
}

void UVSMoverFeatureAgent::OnMoverMovementModeChanged(const FName& PreviousMovementModeName, const FName& NewMovementModeName)
{
	const FGameplayTag PrevMovementModeTag = UVSMoverSystemUtils::GetUnnamedMovementMode(PreviousMovementModeName);
	const FGameplayTag NewMovementModeTag = UVSMoverSystemUtils::GetUnnamedMovementMode(NewMovementModeName);
}