// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSMoverFeature.h"
#include "VSMoverWalkingMovementFeature.generated.h"

/**
 * Walking-mode mover feature extension for movement-input customization.
 */
UCLASS(Abstract, DisplayName = "VS.Feature.Movement.Mover.Mode.Walking")
class VSMOVERSYSTEM_API UVSMoverWalkingMovementFeature : public UVSMoverFeature
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin IMoverInputProducerInterface
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;
	//~ End IMoverInputProducerInterface
};
