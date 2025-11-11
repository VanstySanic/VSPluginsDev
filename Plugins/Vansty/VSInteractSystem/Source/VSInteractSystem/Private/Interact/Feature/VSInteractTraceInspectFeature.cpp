// Copyright VanstySanic. All Rights Reserved.

#include "Interact/Feature/VSInteractTraceInspectFeature.h"
#include "KismetTraceUtils.h"
#include "VSInteractSystemUtils.h"
#include "Classes/Features/VSControlRotationFeature.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Interfaces/VSControlRotationFeatureInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSObjectLibrary.h"
#include "Libraries/VSSetContainerLibrary.h"

UVSInteractTraceInspectFeature::UVSInteractTraceInspectFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInteractTraceInspectFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (GetOwnerActor()->GetClass()->ImplementsInterface(UVSControlRotationFeatureInterface::StaticClass()))
	{
		ControlRotationFeaturePrivate = IVSControlRotationFeatureInterface::Execute_GetControlRotationFeature(GetOwnerActor());
	}
	else
	{
		ControlRotationFeaturePrivate = UVSObjectLibrary::FindFeatureByClassFromObject<UVSControlRotationFeature>(GetOwnerActor());
	}
}

bool UVSInteractTraceInspectFeature::CanTick_Implementation() const
{
	return Super::CanTick_Implementation() && UVSActorLibrary::IsActorNetLocal(GetOwnerActor());
}

void UVSInteractTraceInspectFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	const FGameplayTagContainer& GameplayTags = GameplayTagController->GetGameplayTags();
	if (!IsInteracting() && (DoTraceEntryTagQuery.IsEmpty() || DoTraceEntryTagQuery.Matches(GameplayTags)))
	{
		DoTrace(DeltaTime);
	}
	else if (!TracedInspectiveTargets.IsEmpty())
	{
		UVSInteractFeatureAgent* InteractFeatureAgent = GetInteractFeatureAgent();
		for (TWeakObjectPtr<UVSInteractiveFeatureAgent> InteractiveFeatureAgent : TracedInspectiveTargets)
		{
			InteractFeatureAgent->StopInspectionOnTarget(InteractiveFeatureAgent.Get());
		}
		TracedInspectiveTargets.Empty();
	}
}

void UVSInteractTraceInspectFeature::OnFeatureDeactivated_Implementation()
{
	if (UVSInteractFeatureAgent* InteractFeatureAgent = GetInteractFeatureAgent())
	{
		for (TWeakObjectPtr<UVSInteractiveFeatureAgent> InteractiveFeatureAgent : TracedInspectiveTargets)
		{
			if (InteractiveFeatureAgent.IsValid())
			{
				InteractFeatureAgent->StopInspectionOnTarget(InteractiveFeatureAgent.Get());
			}
		}
	}
	
	Super::OnFeatureDeactivated_Implementation();
}

void UVSInteractTraceInspectFeature::DoTrace(float DeltaTime)
{
	static float TraceDistanceBuffer = 16.f;
	
	UVSInteractFeatureAgent* InteractFeatureAgent = GetInteractFeatureAgent();

	const FVector Origin = GetOwnerActor()->GetActorLocation();
	const FRotator ActorRotation = GetOwnerActor()->GetActorRotation();

	const int32 YawSteps = FMath::FloorToInt((MaxTraceYawAngleRange.Y - MaxTraceYawAngleRange.X) / TraceAngleStep) + 1;
	const int32 PitchSteps = FMath::FloorToInt((MaxTracePitchAngleRange.Y - MaxTracePitchAngleRange.X) / TraceAngleStep) + 1;
	
	FQuat ControllerRotation = FQuat::Identity;
	if (bTryUseControlDirectionInsteadOfActor && ControlRotationFeaturePrivate.IsValid())
	{
		if (bControlDirectionUseOnly2D)
		{
			const FQuat& WorldToUpRotation = FQuat::FindBetweenVectors(FVector::UpVector, GetOwnerActor()->GetActorUpVector());
			FRotator ControlDS = UKismetMathLibrary::ComposeRotators(ControlRotationFeaturePrivate->GetControlRotation(), WorldToUpRotation.Inverse().Rotator());
			ControlDS.Pitch = 0.f;
			ControllerRotation = UKismetMathLibrary::ComposeRotators(ControlDS, WorldToUpRotation.Rotator()).Quaternion();
		}
		else
		{
			ControllerRotation = ControlRotationFeaturePrivate->GetControlRotation().Quaternion();
		}
	}

	FQuat TraceBaseRotation = (bTryUseControlDirectionInsteadOfActor && ControlRotationFeaturePrivate.IsValid()) ? ControllerRotation : ActorRotation.Quaternion();
	FVector TraceBaseDirection = TraceBaseRotation.Vector();

	TArray<UVSInteractiveFeatureAgent*> FoundInteractiveAgents;
	for (int32 PitchIndex = 0; PitchIndex < PitchSteps; ++PitchIndex)
	{
		const float PitchAngle = MaxTracePitchAngleRange.X + TraceAngleStep * PitchIndex;

		for (int32 YawIndex = 0; YawIndex < YawSteps; ++YawIndex)
		{
			const float YawAngle = MaxTraceYawAngleRange.X + TraceAngleStep * YawIndex;

			const FRotator OffsetRotator = FRotator(PitchAngle, YawAngle, 0.f);
			const FQuat WorldQuat = TraceBaseRotation * OffsetRotator.Quaternion();

			const FVector& Direction = WorldQuat.GetForwardVector();
			const FVector End = Origin + Direction * (MaxTraceDistance + TraceDistanceBuffer);

			FHitResult HitResult;
			FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DoTrace), false, GetOwnerActor());

			GetWorld()->LineTraceSingleByChannel(HitResult, Origin, End, TraceChannel, QueryParams);

			if (HitResult.IsValidBlockingHit())
			{
				if (UVSInteractiveFeatureAgent* Agent = UVSInteractSystemUtils::GetInteractiveFeatureAgentFromActor(HitResult.GetActor()))
				{
					if (Agent->IsInspectable(InteractFeatureAgent))
					{
						FoundInteractiveAgents.AddUnique(Agent);
					}
				}
			}

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
			if (bDrawDebugShapes)
			{
				DrawDebugLineTraceSingle(GetWorld(), Origin, End, EDrawDebugTrace::ForOneFrame, HitResult.bBlockingHit, HitResult, FColor::Red, FColor::Green, 3);
			}
#endif
		}
	}

	if (bSortByAngleInsteadOfDistance)
	{
		FoundInteractiveAgents.StableSort([&] (const UVSInteractiveFeatureAgent& A, const UVSInteractiveFeatureAgent& B)
		{
			const FVector& DirectionA = (A.GetOwnerActor()->GetActorLocation() - Origin).GetSafeNormal();
			const FVector& DirectionB = (B.GetOwnerActor()->GetActorLocation() - Origin).GetSafeNormal();
			const float DotA = DirectionA.Dot(TraceBaseDirection);
			const float DotB = DirectionB.Dot(TraceBaseDirection);
			return DotA >= DotB;
		});
	}
	else
	{
		FoundInteractiveAgents.StableSort([&] (const UVSInteractiveFeatureAgent& A, const UVSInteractiveFeatureAgent& B)
		{
			const float DisA = (A.GetOwnerActor()->GetActorLocation() - Origin).Size();
			const float DisB = (B.GetOwnerActor()->GetActorLocation() - Origin).Size();
			return DisA < DisB;
		});
	}

	const TArray<UVSInteractiveFeatureAgent*> CurrentTargets = GetCurrentInspectiveTargets();
	const TArray<UVSInteractiveFeatureAgent*>& TargetsToStopInteraction = UVSSetContainerLibrary::GetArrayDifference<UVSInteractiveFeatureAgent*>(CurrentTargets, FoundInteractiveAgents);
	for (UVSInteractiveFeatureAgent* StopInteraction : TargetsToStopInteraction)
	{
		InteractFeatureAgent->StopInspectionOnTarget(StopInteraction);
	}
	
	TracedInspectiveTargets.Empty();
	for (UVSInteractiveFeatureAgent* FoundInteractiveAgent : FoundInteractiveAgents)
	{
		TracedInspectiveTargets.Add(FoundInteractiveAgent);
		if (!InteractFeatureAgent->IsInspectingOnTarget(FoundInteractiveAgent) && (MaxInspectionNum <= 0 || TracedInspectiveTargets.Num() <= MaxInspectionNum))
		{
			InteractFeatureAgent->TryInspectTarget(FoundInteractiveAgent);
		}
	}
	while (TracedInspectiveTargets.Num() > MaxInspectionNum)
	{
		if (InteractFeatureAgent->IsInspectingOnTarget(TracedInspectiveTargets.Last().Get()))
		{
			InteractFeatureAgent->StopInspectionOnTarget(TracedInspectiveTargets.Last().Get());
		}
		TracedInspectiveTargets.Pop();
	}
}

TArray<UVSInteractiveFeatureAgent*> UVSInteractTraceInspectFeature::GetCurrentInspectiveTargets() const
{
	TArray<UVSInteractiveFeatureAgent*> Targets;
	for (TWeakObjectPtr<UVSInteractiveFeatureAgent> InteractiveFeatureAgent : TracedInspectiveTargets)
	{
		if (InteractiveFeatureAgent.IsValid())
		{
			Targets.Add(InteractiveFeatureAgent.Get());
		}
	}
	return Targets;
}
