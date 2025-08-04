// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/VSCharacterMovementTypes.h"
#include "VSCharacterMovementUtils.generated.h"

class UCharacterMovementComponent;
struct FVSOrientationEvaluateParams;

/**
 * 
 */
UCLASS()
class VSMOVEMENTSYSTEM_API UVSCharacterMovementUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(Blueprintable, Category = "Movement", meta = (DefaultToSelf = "Character", AutoCreateRefTerm = "Params"))
	static bool EvaluateCharacterMovementOrientation(const ACharacter* Character, FRotator& OutRotation, const FVSOrientationEvaluateParams& Params);

	UFUNCTION(Blueprintable, Category = "Movement", meta = (DefaultToSelf = "Character", AutoCreateRefTerm = "NewScale"))
	static void SetCharacterMovementScale(ACharacter* Character, const FVector& NewScale);

	UFUNCTION(Blueprintable, Category = "Movement", meta = (DefaultToSelf = "Character", AutoCreateRefTerm = "NewScale"))
	static void ApplyCharacterMovementScaleDelta(ACharacter* Character, const FVector& NewScale, const FVector& PrevScale);
};
