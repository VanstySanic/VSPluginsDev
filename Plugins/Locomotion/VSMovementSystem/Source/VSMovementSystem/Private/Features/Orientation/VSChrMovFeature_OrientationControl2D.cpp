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

	MovementData.CurrentSettings = DefaultSettings;
	UpdateTagQueryStates(EVSMovementEvent::StateChange_MovementMode);
}

void UVSChrMovFeature_OrientationControl2D::UpdateMovement_Implementation(float DeltaTime)
{
	const FRotator& CurrentRotationWS = GetCharacter()->GetActorRotation();
	const FRotator& WorldToUpRotation = FQuat::FindBetweenNormals(FVector::UpVector, GetUpDirection()).Rotator();
	FRotator EvaluatedRotation = GetCharacter()->GetActorRotation();

	bool bEvaluatedRotation = false;
	if (MovementData.bMatchesTagQuery)
	{
		if (IsMoving2D() && (!MovementData.CurrentSettings.bMovingRequireInput || HasMovementInput2D()))
		{
			for (UVSChrMovFeature_OrientationEvaluator* FindSubFeaturesByClass : GetMovementFeatureAgent()->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
			{
				if (FindSubFeaturesByClass->EvaluateOrientation(EvaluatedRotation, FVSOrientationEvaluateParams(MovementData.CurrentSettings.MovingEvaluateType)))
				{
					bEvaluatedRotation = true;
					break;
				}
			}
		}
		else if (!IsMoving2D())
		{
			for (UVSChrMovFeature_OrientationEvaluator* FindSubFeaturesByClass :GetMovementFeatureAgent()->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
			{
				if (FindSubFeaturesByClass->EvaluateOrientation(EvaluatedRotation, FVSOrientationEvaluateParams(MovementData.CurrentSettings.IdleEvaluateType)))
				{
					bEvaluatedRotation = true;
					break;
				}
			}
		}

		if (bEvaluatedRotation)
		{
			const FRotator& LaggedRotationWS = UVSMathLibrary::RotatorInterpTo(CurrentRotationWS, EvaluatedRotation, DeltaTime, FRotator(MovementData.CurrentSettings.OrientationLagSpeed), false, MovementData.CurrentSettings.OrientationLagMaxTimeSubstepping, WorldToUpRotation);
			const FRotator& AxesedRotation = UVSMathLibrary::RotatorApplyAxes(LaggedRotationWS, LaggedRotationWS, EVSRotatorAxes::PitchYaw, WorldToUpRotation);
			GetCharacter()->SetActorRotation(AxesedRotation);
		}
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
	
	MovementData.bMatchesTagQuery = MovementTagQueries.Matches(GameplayTags, TagEvent);

	if (RefreshQueriedSettingsQueries.Matches(GameplayTags, TagEvent))
	{
		MovementData.CurrentSettings = DefaultSettings;
		for (const auto& QueriedSetting : QueriedSettings)
		{
			if (QueriedSetting.Value.Matches(GameplayTags, TagEvent))
			{
				MovementData.CurrentSettings = QueriedSetting.Key;
				break;
			}
		}
	}
}
