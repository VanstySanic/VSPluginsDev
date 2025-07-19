// Copyright VanstySanic. All Rights Reserved.

#include "VSCharacterMovementUtils.h"
#include "VSCharacterMovementInterface.h"
#include "Components/CapsuleComponent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "Features/Orientation/VSChrMovFeature_OrientationEvaluator.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"

UVSCharacterMovementUtils::UVSCharacterMovementUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSCharacterMovementUtils::EvaluateCharacterMovementOrientation(const ACharacter* Character, FRotator& OutRotation, const FVSOrientationEvaluateParams& Params)
{
	if (!Character || !Character->GetClass()->ImplementsInterface(UVSCharacterMovementInterface::StaticClass())) return false;
	
	for (UVSChrMovFeature_OrientationEvaluator* Evaluator : IVSCharacterMovementInterface::Execute_GetMovementAgentFeature(Character)->FindSubFeaturesByClass<UVSChrMovFeature_OrientationEvaluator>())
	{
		if (Evaluator && Evaluator->EvaluateOrientation(OutRotation, Params)) return true;
	}
	
	UVSChrMovFeature_OrientationEvaluator::StaticClass()->GetDefaultObject<UVSChrMovFeature_OrientationEvaluator>()->EvaluateOrientation(OutRotation, Params);
	return false;
}
