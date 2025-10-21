// Copyright VanstySanic. All Rights Reserved.

#include "Features/Details/VSChrMovAnimFeature_AimOffset.h"
#include "VSCharacterMovementUtils.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "Types/VSChrMovOrientationTypes.h"

UVSChrMovAnimFeature_AimOffset::UVSChrMovAnimFeature_AimOffset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovAnimFeature_AimOffset::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	AnimData.CurrentOrientationEvaluateType = DefaultOrientationEvaluateType;
	AnimData.CurrentAimOffsetBlendSpace = ModdedAimOffsetBlendSpaces.FindRef(GetMovementMode());
	UpdateTagQueryStates(EVSMovementEvent::StateChange_MovementMode);
}

void UVSChrMovAnimFeature_AimOffset::UpdateAnimationThreadSafe_Implementation(float DeltaTime)
{
	Super::UpdateAnimationThreadSafe_Implementation(DeltaTime);

	if (!AnimData.bMatchesTagQuery)
	{
		AnimData.AimOffsetAngle = FVector2D::ZeroVector;
	}
	else
	{
		FVSMovementOrientationEvaluateParams OrientationEvaluateParams(AnimData.CurrentOrientationEvaluateType);
		OrientationEvaluateParams.bReturnRotationInSpace2D = false;
		FRotator EvaluatedRotation = GetMovementFeatureAgent()->EvaluateOrientation(OrientationEvaluateParams);
		const FRotator& RotationRS = GetCharacter()->GetActorTransform().InverseTransformRotation(EvaluatedRotation.Quaternion()).Rotator();
		AnimData.AimOffsetAngle.X = RotationRS.Yaw;
		AnimData.AimOffsetAngle.Y = RotationRS.Pitch;
	}
}

void UVSChrMovAnimFeature_AimOffset::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);

	UpdateTagQueryStates(TagEvent);
}

void UVSChrMovAnimFeature_AimOffset::UpdateTagQueryStates(const FGameplayTag& TagEvent)
{
	const UVSGameplayTagController* GameplayTagSet = GetGameplayTagController();
	FGameplayTagContainer GameplayTags;
	GameplayTagSet->GetOwnedGameplayTags(GameplayTags);
	
	AnimData.bMatchesTagQuery = EnabledTagQuery.Matches(GameplayTags);
	if (!AnimData.bMatchesTagQuery)
	{
		AnimData.AimOffsetAngle = FVector2D::ZeroVector;
	}
	
	if (RefreshQueriedOrientationEvaluateTypeTagQuery.Matches(TagEvent, GameplayTags))
	{
		AnimData.CurrentOrientationEvaluateType = DefaultOrientationEvaluateType;
		for (const auto& TaggedTargetOrientationEvaluateType : TagQueriedOrientationEvaluateTypes)
		{
			if (TaggedTargetOrientationEvaluateType.Value.Matches(TagEvent, GameplayTags))
			{
				AnimData.CurrentOrientationEvaluateType = TaggedTargetOrientationEvaluateType.Key;
				break;
			}
		}
	}

	if (TagEvent == EVSMovementEvent::StateChange_MovementMode)
	{
		AnimData.CurrentAimOffsetBlendSpace = ModdedAimOffsetBlendSpaces.FindRef(GetMovementMode());
	}
}