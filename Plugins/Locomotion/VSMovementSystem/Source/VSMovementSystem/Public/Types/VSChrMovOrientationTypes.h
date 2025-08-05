// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCharacterMovementTags.h"
#include "Types/VSGameplayTypes.h"
#include "Types/VSCharacterMovementTypes.h"
#include "UObject/Object.h"
#include "VSChrMovOrientationTypes.generated.h"

struct FGameplayTag;

USTRUCT(BlueprintType)
struct FVSOrientationControlSettings2D
{
	GENERATED_BODY()

	friend uint32 GetTypeHash(const FVSOrientationControlSettings2D& Settings)
	{
		const uint32 HashA = HashCombine(GetTypeHash(Settings.MovingEvaluateType), GetTypeHash(Settings.IdleEvaluateType));
		const uint32 HashB = HashCombine(GetTypeHash(Settings.OrientationLagSpeed), GetTypeHash(Settings.OrientationLagSpeed));
		return HashCombine(HashA, HashB);
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVSMovementOrientationEvaluateType MovingEvaluateType = FVSMovementOrientationEvaluateType(EVSMovementRelatedOrientationType::Control);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVSMovementOrientationEvaluateType IdleEvaluateType;
	
	/** Lag the moving orientation 2D to the desired. 0.f means no lag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OrientationLagSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OrientationLagMaxTimeSubstepping = 0.0166667f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMovingRequireInput = true;
};

#pragma region TurnInPlace2D
USTRUCT(BlueprintType)
struct FVSTurnInPlaceSettings2D : public FTableRowBase
{
	GENERATED_BODY()

	VSMOVEMENTSYSTEM_API bool IsValid() const;

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

/** Turn in place data that is unchanged during process used in animation. This may be replicated. */
USTRUCT(BlueprintType)
struct FVSTurnInPlaceSnappedParams2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSDataTableRowHandleWrap SettingsRow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSDataTableRowHandleWrap AnimRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRateMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NotReplicated)
	int32 ActionID = INDEX_NONE;
};
#pragma endregion 

