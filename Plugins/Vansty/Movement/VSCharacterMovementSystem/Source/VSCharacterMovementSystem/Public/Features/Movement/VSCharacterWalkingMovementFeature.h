// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "VSCharacterWalkingMovementFeature.generated.h"

/**
 * Walking-mode character movement feature extension.
 */
UCLASS(Abstract, DisplayName = "VS.Feature.Movement.Character.Mode.Walking")
class VSCHARACTERMOVEMENTSYSTEM_API UVSCharacterWalkingMovementFeature : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()
};
