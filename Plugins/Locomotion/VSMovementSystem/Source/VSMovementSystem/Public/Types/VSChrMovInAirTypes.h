// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "VSChrMovInAirTypes.generated.h"

class UVSCharacterMovementFeatureAgent;


USTRUCT(BlueprintType)
struct FVSFixedPointLeapLimits : public FTableRowBase
{
	GENERATED_BODY()

	bool IsValid() const;
	bool Matches(const UVSCharacterMovementFeatureAgent* Agent) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRange2D = FVector2D(0, 65536.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRangeZ = FVector2D(0, 65536.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityAngleRangePitch = FVector2D(-90.f, 90.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityAngleRangeYaw = FVector2D(-180.f, 180.f);

	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery MovementTagQuery;
};

USTRUCT(BlueprintType)
struct FVSFixedPointLeapSettings : public FTableRowBase
{
	GENERATED_BODY()

	bool IsValid() const;

	/** Randomized. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "/Script/VSPluginsCore.VSAnimSequenceReference"))
	TArray<FDataTableRowHandle> AnimRows;

	/** Below 0.f means default capsule radius. Unscaled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CapsuleHalfHeight = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSFixedPointLeapLimits Limits;
};

USTRUCT(BlueprintType)
struct FVSFixedPointLeapSnappedParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle SettingsRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle AnimRow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OriginalTargetRootPointWS = FVector::ZeroVector;

	/** This might be lost because of non-replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform StartComponentTransformWS = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ServerSideServerStartTime = 0.f;
};

USTRUCT(BlueprintType)
struct FVSFixedPointLeapCachedParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClientSideServerStartTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimPlayRate = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionID = INDEX_NONE;
};