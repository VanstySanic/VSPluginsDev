// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSMovementAnimUtils.generated.h"

class UVSCharacterMovementAnimFeature;
struct FVSKeyedAnimSequenceMapContainer;
struct FVSGameplayTagKey;

/**
 * 
 */
UCLASS()
class VSMOVEMENTANIM_API UVSMovementAnimUtils : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tools", meta = (BlueprintThreadSafe, AutoCreateRefTerm = "PrevDirection"))
	static FGameplayTag CalcAnimDirectionByAngle2D(const float Angle, const float BufferAngle = 3.f, const FGameplayTag& PrevDirection = FGameplayTag());

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (BlueprintThreadSafe, AutoCreateRefTerm = "Current, Base, UpDirection, PrevDirection"))
	static FGameplayTag CalcAnimDirectionByTwoVector2D(const FVector& Current, const FVector& RelativeTo, const FVector& UpDirection, const float BufferAngle = 3.f, const FGameplayTag& PrevDirection = FGameplayTag());

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tools", DisplayName = "GetAnimSequence", meta = (BlueprintThreadSafe, AutoCreateRefTerm = "KeyedAnimSequenceMaps, GameplayTagKey"))
	static UAnimSequence* GetAnimSequenceFromKeyedMapContainer(const FVSKeyedAnimSequenceMapContainer& KeyedAnimSequenceMaps, const FVSGameplayTagKey& GameplayTagKey);

	UFUNCTION(BlueprintCallable, Category = "Tools", meta = (BlueprintThreadSafe))
	static void UpdateAnimFeaturesThreadSafe(const TArray<UVSCharacterMovementAnimFeature*>& AnimFeatures, float DeltaTime);
};