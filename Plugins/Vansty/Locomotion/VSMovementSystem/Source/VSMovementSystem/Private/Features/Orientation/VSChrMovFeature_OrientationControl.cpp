// Copyright VanstySanic. All Rights Reserved.

#include "Features/Orientation/VSChrMovFeature_OrientationControl.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "Libraries/VSMathLibrary.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/VSChrMovOrientationTypes.h"

UVSChrMovFeature_OrientationControl::UVSChrMovFeature_OrientationControl(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UVSChrMovFeature_OrientationControl::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	MovementData.CurrentSettings = DefaultSettings;
	UpdateTagQueryStates(EVSMovementEvent::StateChange_MovementMode);
}

void UVSChrMovFeature_OrientationControl::UpdateMovement_Implementation(float DeltaTime)
{
	const FRotator& CurrentRotationWS = GetCharacter()->GetActorRotation();
	const FRotator& WorldToUpRotation = FQuat::FindBetweenNormals(FVector::UpVector, GetUpDirection()).Rotator();
	FRotator EvaluatedRotation = GetCharacter()->GetActorRotation();
	
	if (MovementData.bMatchesTagQuery)
	{
		FVSMovementOrientationEvaluateParams EvaluateParams = FVSMovementOrientationEvaluateParams(MovementData.CurrentSettings.EvaluateType);
		EvaluateParams.bReturnRotationInSpace2D = MovementData.CurrentSettings.bControlOnly2D;
		EvaluatedRotation = GetMovementFeatureAgent()->EvaluateOrientation(EvaluateParams);

		const FRotator& LaggedRotationWS = UVSMathLibrary::RotatorInterpTo(CurrentRotationWS, EvaluatedRotation, DeltaTime, FRotator(MovementData.CurrentSettings.OrientationLagSpeed), false, MovementData.CurrentSettings.OrientationLagMaxTimeSubstepping, WorldToUpRotation);
		const FRotator& AxesedRotation = UVSMathLibrary::RotatorApplyAxes(LaggedRotationWS, LaggedRotationWS, EVSRotatorAxes::PitchYaw, WorldToUpRotation);
		GetCharacter()->SetActorRotation(AxesedRotation);
	}
}

void UVSChrMovFeature_OrientationControl::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	UpdateTagQueryStates(TagEvent);
}

void UVSChrMovFeature_OrientationControl::UpdateTagQueryStates(const FGameplayTag& TagEvent)
{
	const UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	const FGameplayTagContainer& GameplayTags = GameplayTagController->GetGameplayTags();
	
	MovementData.bMatchesTagQuery = MovementTagQuery.Matches(TagEvent, GameplayTags);

	if (RefreshQueriedSettingsTagQuery.Matches(TagEvent, GameplayTags))
	{
		MovementData.CurrentSettings = DefaultSettings;
		for (const auto& QueriedSetting : TagQueriedSettings)
		{
			if (QueriedSetting.Value.Matches(TagEvent, GameplayTags))
			{
				MovementData.CurrentSettings = QueriedSetting.Key;
				break;
			}
		}
	}
}
