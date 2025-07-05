// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_OrientationControl2D.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSMathLibrary.h"
#include "Types/VSChrMovOrientationTypes.h"

UVSChrMovFeature_OrientationControl2D::UVSChrMovFeature_OrientationControl2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UVSChrMovFeature_OrientationControl2D::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	MovementData.CurrentMovingOrientationEvaluateType = DefaultMovingOrientationEvaluateType;
	MovementData.CurrentIdleOrientationEvaluateType = DefaultIdleOrientationEvaluateType;
	UpdateTagQueryStates(EVSMovementEvent::StateChange_MovementMode);
}

void UVSChrMovFeature_OrientationControl2D::UpdateMovement_Implementation(float DeltaTime)
{
	const FRotator& CurrentRotationWS = GetCharacter()->GetActorRotation();
	const FRotator& WorldToUpRotation = FQuat::FindBetweenNormals(FVector::UpVector, GetUpDirection()).Rotator();
	FRotator EvaluatedRotation = GetCharacter()->GetActorRotation();
	bool bEvaluatedRotation = false;
	if (MovementData.bMatchesMovingTagQuery && IsMoving2D() && (!bMovingRequireInput || HasMovementInput2D()))
	{
		for (UVSChrMovFeature_OrientationEvaluator* FindSubFeaturesByClass : GetMovementFeatureAgent()->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
		{
			if (FindSubFeaturesByClass->EvaluateOrientation(EvaluatedRotation, FVSOrientationEvaluateParams(MovementData.CurrentMovingOrientationEvaluateType)))
			{
				bEvaluatedRotation = true;
				break;
			}
		}
	}
	else if (MovementData.bMatchesIdleTagQuery && !IsMoving2D())
	{
		for (UVSChrMovFeature_OrientationEvaluator* FindSubFeaturesByClass :GetMovementFeatureAgent()->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
		{
			if (FindSubFeaturesByClass->EvaluateOrientation(EvaluatedRotation, FVSOrientationEvaluateParams(MovementData.CurrentIdleOrientationEvaluateType)))
			{
				bEvaluatedRotation = true;
				break;
			}
		}
	}
	if (bEvaluatedRotation)
	{
		const FRotator& LaggedRotationWS = UVSMathLibrary::RotatorInterpTo(CurrentRotationWS, EvaluatedRotation, DeltaTime, FRotator(MovingOrientationLagSpeed), false, LagMaxTimeSubstepping, WorldToUpRotation);
		const FRotator& AxesedRotation = UVSMathLibrary::RotatorApplyAxes(LaggedRotationWS, LaggedRotationWS, EVSRotatorAxes::PitchYaw, WorldToUpRotation);
		GetCharacter()->SetActorRotation(AxesedRotation);
	}
}

void UVSChrMovFeature_OrientationControl2D::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	UpdateTagQueryStates(TagEvent);
}

void UVSChrMovFeature_OrientationControl2D::UpdateTagQueryStates(const FGameplayTag& TagEvent)
{
	const UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	FGameplayTagContainer GameplayTags;
	GameplayTagController->GetOwnedGameplayTags(GameplayTags);
	
	MovementData.bMatchesMovingTagQuery = MovingTagQueries.Matches(GameplayTags, TagEvent);
	MovementData.bMatchesIdleTagQuery = IdleTagQueries.Matches(GameplayTags, TagEvent);

	if (RefreshQueriedMovingOrientationEvaluateTypeQueries.Matches(GameplayTags, TagEvent))
	{
		MovementData.CurrentMovingOrientationEvaluateType = DefaultMovingOrientationEvaluateType;
		for (const auto& QueriedMovingOrientationEvaluateType : QueriedMovingOrientationEvaluateTypes)
		{
			if (QueriedMovingOrientationEvaluateType.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentMovingOrientationEvaluateType = QueriedMovingOrientationEvaluateType.Key;
				break;
			}
		}
	}

	if (RefreshQueriedIdleOrientationEvaluateTypeQueries.Matches(GameplayTags, TagEvent))
	{
		MovementData.CurrentIdleOrientationEvaluateType = DefaultIdleOrientationEvaluateType;
		for (const auto& TaggedIdleOrientationEvaluateType : QueriedIdleOrientationEvaluateTypes)
		{
			if (TaggedIdleOrientationEvaluateType.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentIdleOrientationEvaluateType = TaggedIdleOrientationEvaluateType.Key;
				break;
			}
		}
	}
}
