// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_OrientationControl2D.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "GameFramework/Character.h"
#include "Libraries/VSMathLibrary.h"
#include "Types/VSChrMovOrientationTypes.h"

UVSChrMovFeature_OrientationControl2D::UVSChrMovFeature_OrientationControl2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UVSChrMovFeature_OrientationControl2D::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	UpdateTagQueryStates(FGameplayTag::EmptyTag);
	MovementData.CurrentMovingOrientationEvaluateType = bOverrideMovingOrientationEvaluateType ? OverridenMovingOrientationEvaluateType : MovementData.CurrentTaggedMovingOrientationEvaluateType;
	MovementData.CurrentIdleOrientationEvaluateType = bOverrideIdleOrientationEvaluateType ? OverridenIdleOrientationEvaluateType : MovementData.CurrentTaggedIdleOrientationEvaluateType;
}

void UVSChrMovFeature_OrientationControl2D::UpdateMovement_Implementation(float DeltaTime)
{
	const FRotator& CurrentRotationWS = GetCharacter()->GetActorRotation();
	const FRotator& WorldToUpRotation = FQuat::FindBetweenNormals(FVector::UpVector, GetUpDirection()).Rotator();
	FRotator EvaluatedRotation = GetCharacter()->GetActorRotation();
	bool bEvaluatedRotation = false;
	if (MovementData.bMatchesMovingTagQuery && (IsMoving2D() || HasMovementInput()) && (!bMovingRequireInput || HasMovementInput2D()))
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
		GetCharacter()->SetActorRotation(UVSMathLibrary::RotatorApplyAxes(LaggedRotationWS, LaggedRotationWS, EVSRotatorAxes::PitchYaw, WorldToUpRotation));
	}
}

void UVSChrMovFeature_OrientationControl2D::OnMovementTagsUpdated_Implementation()
{
	UpdateTagQueryStates(FGameplayTag::EmptyTag);
}

void UVSChrMovFeature_OrientationControl2D::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	UpdateTagQueryStates(TagEvent);
}

void UVSChrMovFeature_OrientationControl2D::SetOverridenMovingOrientationEvaluateType(const FGameplayTag& Type)
{
	OverridenMovingOrientationEvaluateType = Type;
	MovementData.CurrentMovingOrientationEvaluateType = bOverrideMovingOrientationEvaluateType ? OverridenMovingOrientationEvaluateType : MovementData.CurrentTaggedMovingOrientationEvaluateType;
}

void UVSChrMovFeature_OrientationControl2D::SetOverrideMovingOrientationEvaluateType(const bool bOverride)
{
	if (bOverrideMovingOrientationEvaluateType == bOverride) return;
	bOverrideMovingOrientationEvaluateType = bOverride;
	MovementData.CurrentMovingOrientationEvaluateType = bOverrideMovingOrientationEvaluateType ?  OverridenMovingOrientationEvaluateType : MovementData.CurrentTaggedMovingOrientationEvaluateType;
}

void UVSChrMovFeature_OrientationControl2D::SetOverridenIdleOrientationEvaluateType(const FGameplayTag& Type)
{
	OverridenIdleOrientationEvaluateType = Type;
	MovementData.CurrentIdleOrientationEvaluateType = bOverrideIdleOrientationEvaluateType ? OverridenIdleOrientationEvaluateType : MovementData.CurrentTaggedIdleOrientationEvaluateType;
}

void UVSChrMovFeature_OrientationControl2D::SetOverrideIdleOrientationEvaluateType(const bool bOverride)
{
	if (bOverrideIdleOrientationEvaluateType == bOverride) return;
	bOverrideIdleOrientationEvaluateType = bOverride;
	MovementData.CurrentIdleOrientationEvaluateType = bOverrideIdleOrientationEvaluateType ? OverridenIdleOrientationEvaluateType : MovementData.CurrentTaggedIdleOrientationEvaluateType;
}


void UVSChrMovFeature_OrientationControl2D::UpdateTagQueryStates(const FGameplayTag& TagEvent)
{
	const UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	FGameplayTagContainer GameplayTags;
	GameplayTagController->GetOwnedGameplayTags(GameplayTags);
	MovementData.bMatchesMovingTagQuery = MovingTagQuerySettings.Matches(GameplayTags, TagEvent);
	MovementData.bMatchesIdleTagQuery = IdleTagQuerySettings.Matches(GameplayTags, TagEvent);

	if (RefreshQueriedMovingOrientationEvaluateTypeQuery.Matches(GameplayTags, TagEvent))
	{
		for (const auto& TaggedMovingOrientationEvaluateType : QueriedMovingOrientationEvaluateTypes)
		{
			if (TaggedMovingOrientationEvaluateType.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentTaggedMovingOrientationEvaluateType = TaggedMovingOrientationEvaluateType.Key;
				break;
			}
		}
		if (!bOverrideMovingOrientationEvaluateType)
		{
			MovementData.CurrentMovingOrientationEvaluateType = MovementData.CurrentTaggedMovingOrientationEvaluateType;
		}
	}

	if (RefreshQueriedIdleOrientationEvaluateTypeQuery.Matches(GameplayTags, TagEvent))
	{
		for (const auto& TaggedIdleOrientationEvaluateType : QueriedIdleOrientationEvaluateTypes)
		{
			if (TaggedIdleOrientationEvaluateType.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentTaggedIdleOrientationEvaluateType = TaggedIdleOrientationEvaluateType.Key;
				break;
			}
		}
		if (!bOverrideIdleOrientationEvaluateType)
		{
			MovementData.CurrentIdleOrientationEvaluateType = MovementData.CurrentTaggedIdleOrientationEvaluateType;
		}
	}
}
