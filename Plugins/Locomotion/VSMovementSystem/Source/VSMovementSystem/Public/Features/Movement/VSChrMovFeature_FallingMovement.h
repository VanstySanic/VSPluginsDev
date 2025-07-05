// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "VSChrMovFeature_FallingMovement.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.Falling")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_FallingMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;


private:
	struct FMovementData
	{
		int32 CachedJumpCount = 0;
	} MovementData;
};
