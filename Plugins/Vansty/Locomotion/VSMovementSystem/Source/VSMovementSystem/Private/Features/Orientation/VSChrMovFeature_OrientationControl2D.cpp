// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_OrientationControl2D.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
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
	
	if (MovementData.bMatchesTagQuery)
	{
		if (IsMoving2D() && (!MovementData.CurrentSettings.bMovingRequireInput || HasMovementInput2D()))
		{
			EvaluatedRotation = GetMovementFeatureAgent()->EvaluateOrientation(FVSMovementOrientationEvaluateParams(MovementData.CurrentSettings.MovingEvaluateType));
		}
		else if (!IsMoving2D())
		{
			EvaluatedRotation = GetMovementFeatureAgent()->EvaluateOrientation(FVSMovementOrientationEvaluateParams(MovementData.CurrentSettings.IdleEvaluateType));
		}

		const FRotator& LaggedRotationWS = UVSMathLibrary::RotatorInterpTo(CurrentRotationWS, EvaluatedRotation, DeltaTime, FRotator(MovementData.CurrentSettings.OrientationLagSpeed), false, MovementData.CurrentSettings.OrientationLagMaxTimeSubstepping, WorldToUpRotation);
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
