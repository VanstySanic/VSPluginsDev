// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "VSChrMovFeature_OrientationEvaluator.generated.h"

class UCameraComponent;
struct FVSOrientationEvaluateParams;

/**
 * Evaluates the orientation related to the character by type.
 * Other features may rely on this.
 * You can add multiple evaluators into the features.
 */
UCLASS(Abstract)
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_OrientationEvaluator : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()
	
	/**
	 * Evaluate a world space orientation by params and current states (if no named params assigned).
	 * If not handled, please set the out rotation to the character's rotation.
	 * @param OutRotation It will be set to the character's rotation by default.
	 * @return Whether the condition is handled.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Orientation", meta = (AutoCreateRefTerm = "Params"))
	bool EvaluateOrientation(FRotator& OutRotation, const FVSOrientationEvaluateParams& Params);
};

UCLASS(DisplayName = "Feature.ChrMov.Orientation.Evaluator.Common")
class VSMOVEMENTSYSTEM_API UVSLS_ChrMovFeature_OrientationEvaluator_Common : public UVSChrMovFeature_OrientationEvaluator
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool EvaluateOrientation_Implementation(FRotator& OutRotation, const FVSOrientationEvaluateParams& Params) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	bool bAdjustMovementAgainstWall2D = true;


	
	UPROPERTY(BlueprintReadWrite, Category = "Orientation")
	TWeakObjectPtr<USceneComponent> AimingTargetComponent;

	/** Used when AimingTargetComponent is not valid */
	UPROPERTY(BlueprintReadWrite, Category = "Orientation")
	FVector AimTargetPoint = FVector::ZeroVector;
};