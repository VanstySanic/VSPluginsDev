// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCharacterMovementTags.h"
#include "UObject/Object.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovGroundedTypes.generated.h"

class UVSCharacterMovementFeatureAgent;
class UVSCharacterMovementFeature;

USTRUCT(BlueprintType)
struct FVSLandRecoveryLimits
{
	GENERATED_BODY()

	FVSLandRecoveryLimits()
	{
		AllowedPrevMovementModes = TArray<FGameplayTag>
			{
				EVSMovementMode::Falling,
				EVSMovementMode::FixedPointLeap,
				EVSMovementMode::MantlingOrVaulting,
				EVSMovementMode::WallRunning,
			};
	}

	bool IsValid() const;
	bool Validate();
	bool Matches(const UVSCharacterMovementFeatureAgent* Agent) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRange2D = FVector2D(0, 65536.f);

	/** This will use the last updated speed before reaching ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRangeBeforeLandingZ = FVector2D(0, 65536.f);

	/**
	 * If you are using orientation evaluator, this will be the angle relative to the evaluated orientation,
	 * otherwise, the character's rotation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityAngleRangePitch = FVector2D(-90.f, 0.f);

	/**
	 * If you are using orientation evaluator, this will be the angle relative to the evaluated orientation,
	 * otherwise, the character's rotation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityAngleRangeBeforeLandingYaw = FVector2D(-180.f, 180.f);

	/** Empty as matching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> AllowedPrevMovementModes;
};

UENUM(BlueprintType)
namespace EVSLandRecoveryAnimPlayPolicy
{
	enum Type
	{
		None						UMETA(Hidden),
		PlayAsAdditive,
		PlayWithMovement,
		PlayWithoutMovement,
	};
}

USTRUCT(BlueprintType)
struct FVSLandRecoverySettings : public FTableRowBase
{
	GENERATED_BODY()
	
	bool IsValid() const;

	/** This will be randomized. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "/Script/VSPluginsCore.VSAnimSequenceReference"))
	FDataTableRowHandle RecoveryAnimRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSLandRecoveryAnimPlayPolicy::Type> AnimPlayPolicy = EVSLandRecoveryAnimPlayPolicy::PlayAsAdditive;

	/** <SpeedRangeZ.X, SpeedRangeZ.Y, AlphaRange.X, AlphaRange.Y> */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 AdditiveAlphaClampBySpeedZ = FVector4(0.f, 1000.f, 0.1f, 1.f);

	/** By state. If no query is matched, 1.f will be taken. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<float, FGameplayTagQuery> AdditiveAlphaMultipliers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSLandRecoveryLimits Limits;
};

/** Params that needs to pass to animation module. */
USTRUCT(BlueprintType)
struct FVSLandRecoveryAnimParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVSDataTableRowHandleWrap SettingsRow;

	UPROPERTY(BlueprintReadOnly)
	FDataTableRowHandle AnimRow;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EVSLandRecoveryAnimPlayPolicy::Type> AnimPlayPolicy = EVSLandRecoveryAnimPlayPolicy::None;

	UPROPERTY(BlueprintReadOnly)
	float AdditiveAlpha = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	float PlayRateMultiplier = 1.f;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ActionID = 0;
};
