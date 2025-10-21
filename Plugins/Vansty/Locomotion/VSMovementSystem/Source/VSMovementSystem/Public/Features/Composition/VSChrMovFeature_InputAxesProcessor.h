// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSMathTypes.h"
#include "VSChrMovFeature_InputAxesProcessor.generated.h"

/**
 * Process movement input axes in world space.
 * You may override the GetDesiredMovementInput to customize the behavior.
 * You can also add gameplay tag queries to control more precisely.
 */
UCLASS(DisplayName = "Feature.ChrMov.Composition.InputAxesProcessor")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_InputAxesProcessor : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (AutoCreateRefTerm = "Axes"))
	void ProcessInputAxes(const FVector& Axes, EVSAxesSwizzle::Type Swizzle = EVSAxesSwizzle::YXZ);

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (AutoCreateRefTerm = "MovementInput"))
	void AddMovementInput(const FVector& MovementInput);

	/** Get the world space movement input. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input", meta = (AutoCreateRefTerm = "Axes"))
	FVector GetDesiredMovementInput(const FVector& Axes, EVSAxesSwizzle::Type Swizzle = EVSAxesSwizzle::YXZ);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (AutoCreateRefTerm = "Axes2D"))
	FVector GetMovementInputForCommonMovement2D(const FVector2D& Axes2D, bool bSwizzleAxesXY = true);
	
};
