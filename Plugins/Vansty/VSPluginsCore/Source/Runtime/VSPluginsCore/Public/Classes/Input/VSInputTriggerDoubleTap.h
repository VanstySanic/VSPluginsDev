// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "VSInputTriggerDoubleTap.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Double Tap")
class VSPLUGINSCORE_API UVSInputTriggerDoubleTap : public UInputTriggerTimedBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual ETriggerType GetTriggerType_Implementation() const override { return ETriggerType::Implicit; }
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;

public:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
	float SecondTapTimeThreshold = 0.2f;

private:
	bool bPendingTrigger = false;
	float FirstTapAccumulatedTime = 0.f;
};
