// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCharacterMovementTags.h"
#include "Types/Animation/VSAnimSequenceReference.h"
#include "UObject/Object.h"
#include "VSChrMovOrientationTypes.generated.h"

struct FGameplayTag;

#pragma region Evaluator
USTRUCT(BlueprintType)
struct FVSOrientationEvaluateCommonParamNames
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Velocity = FName("Velocity");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MovementInput = FName("Input");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UpDirection = FName("DownDirection");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AimTargetPoint = FName("AimTargetPoint");
};

USTRUCT(BlueprintType)
struct FVSOrientationEvaluateNamedParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FVector> VectorParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FRotator> RotatorParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, AActor*> ActorParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, USceneComponent*> ComponentParams;
};

USTRUCT(BlueprintType)
struct FVSOrientationEvaluateParams
{
	GENERATED_BODY()

	FVSOrientationEvaluateParams(const FGameplayTag& Type = FGameplayTag::EmptyTag)
		: Type(Type)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Type = EVSOrientationEvaluateType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSOrientationEvaluateNamedParams NamedParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMovementAgainstWallAdjustment2D = true;
	
	/** If true, return the gravity space 2d rotation instead of the 3d rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReturnRotationInSpace2D = true;
};
#pragma endregion

#pragma region TurnInPlace2D
USTRUCT(BlueprintType)
struct VSMOVEMENTSYSTEM_API FVSTurnInPlaceSettings2D : public FTableRowBase
{
	GENERATED_BODY()

	bool IsValid() const;

	/** Turn in place is allowed when the target orientation is AngleThreshold delta from the character rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AngleThreshold = 45.f;

	/** The turn in place will trigger after a delay time when meet with the requirements. Depending on the turning angle, from AngleThreshold to 180.f. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TriggerDelayTimeRange = FVector2D(0.2, 0.2);

	/** Map clamp the play rate by angle.  */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FVector4 PlayRateMultiplierByAngle = FVector4(0.0, 180.0, 1.0, 1.0);
	
	/** If true, the character will rotate entirely following the anim rotation. Otherwise, the rotation amount will be scaled as needed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotationNoScale = false;
	
	/** <AngleRange, Animation> */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "/Script/VSPluginsCore.VSAnimSequenceReference"))
	TMap<FVector2D, FDataTableRowHandle> AngledAnims;
};

/** Data that needs to pass to the animation module. */
USTRUCT(BlueprintType)
struct FVSTurnInPlaceAnimParams2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle AnimRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRateMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionID = 0;
};
#pragma endregion 

