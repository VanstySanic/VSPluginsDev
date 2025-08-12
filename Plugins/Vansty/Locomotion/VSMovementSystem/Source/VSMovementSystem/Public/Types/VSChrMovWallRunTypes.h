// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSCharacterMovementTypes.h"
#include "Types/VSGameplayTypes.h"
#include "UObject/Object.h"
#include "VSChrMovWallRunTypes.generated.h"

class UVSCharacterMovementFeatureAgent;

USTRUCT(BlueprintType)
struct FVSWallRunLimits
{
	GENERATED_BODY()

	FVSWallRunLimits()
	{
		WallComponentQuery.ObjectTypes =
		{
			ObjectTypeQuery1,
			ObjectTypeQuery2,
		};
	}
	
	bool IsValid() const;

	/** X for descending, and Y for entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D RootHeightToGroundThresholdRange = FVector2D(36.f, 64.f);
	
	/**
	 * The character's forward vector should be in that angle against the target wall movement direction.
	 * Facing wall side positive.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D FacingMovementAngleRange2D = FVector2D(-15.f, 60.f);
	
	/** The character's velocity towards the target movement direction must be in that range. Facing wall side positive. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityTowardsMovementAngleRange2D = FVector2D(-15.f, 60.f);

	/** The character's input towards the target movement direction must be in that range. Facing wall side positive. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InputTowardsMovementAngleRange2D = FVector2D(-15.f, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequireMovementInput2D = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSSceneComponentQuery WallComponentQuery;

	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery MovementTagQuery;
};

USTRUCT(BlueprintType)
struct FVSWallRunAnims : public FTableRowBase
{
	GENERATED_BODY()
	
	/** Only used when start from ground. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle StartAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> CycleAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle EndAnim;
};

USTRUCT(BlueprintType)
struct FVSWallRunSettings : public FTableRowBase
{
	GENERATED_BODY()
	
	bool IsValid() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVSWallRunAnims LeftAnims;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVSWallRunAnims RightAnims;

	/** The Z axis is especially for movement from ground. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector WallTraceOffset = FVector(128.0, 72.0, 72);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RootFootHeightOffset = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RootHandHeightOffset = 108.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CycleDistanceToWall = 50.f;

	/** Use default value if below 0.f. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CapsuleHalfHeight = 0.f;
	
	/** Used in cycle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CycleSpeed = 600.f;

	// /** Used to accelerate and brake. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float CycleAccelerationSize = 1024.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSWallRunLimits Limits;
};

/** Snapped params to define a wall run movement and its animation. Replicated. */
USTRUCT(BlueprintType)
struct FVSWallRunSnappedParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle SettingsRow;
	
	/** This might be lost because of non-replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;

	/** This might be lost because of non-replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ComponentName = NAME_None;
	
	/** Safe during replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform StartComponentTransform = FTransform::Identity;

	/** Left : 0 / Right : 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLeftOrRight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFromGround = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartMovementDirection2DRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartWallPointRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartWallRootPointRS = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartWallNormal2DRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ServerSideServerStartTime = 0.f;
};

USTRUCT(BlueprintType)
struct FVSWallRunCachedParams
{
	GENERATED_BODY()

	/** This might be lost because of non-replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UPrimitiveComponent> Component;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClientSideServerStartTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartRootLocationRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartReachWallRootLocationRS = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AnimStartMovementCurveValues = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AnimStartReachWallMovementCurveValues = FVector::ZeroVector;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AnimStartSettleMovementCurveValues = FVector::ZeroVector;;
	
	/** This is for animation, used to restart the anim action. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionID = INDEX_NONE;
};