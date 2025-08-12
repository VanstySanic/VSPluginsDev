// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/VSCharacterMovementTags.h"
#include "UObject/Object.h"
#include "VSChrMovAnimInAirTypes.generated.h"

class UVSCharacterMovementAnimFeatureAgent;
class UCharacterMovementComponent;

USTRUCT(BlueprintType)
struct FVSFallingMovementAnimLimits
{
	GENERATED_BODY()

	FVSFallingMovementAnimLimits()
	{
		AllowedPrevMovementModes =
			{
				EVSMovementMode::Walking,
				EVSMovementMode::NavWalking,
				EVSMovementMode::Falling,
				EVSMovementMode::FixedPointLeap,
				EVSMovementMode::MantlingOrVaulting,
				EVSMovementMode::WallRunning,
			};
	}
	
	bool IsValid() const;
	bool Matches(const UVSCharacterMovementAnimFeatureAgent* Agent) const;

	/** Unsigned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRange2D = FVector2D(0.f, 65536.f);

	/** Signed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedRangeZ = FVector2D(-65535.f, 65536.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityAngleRangePitch = FVector2D(-90.f, 90.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VelocityAngleRangeYaw = FVector2D(-180.f, 180.f);

	/** Empty as all. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> AllowedPrevMovementModes;

	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery MovementTagQuery;
};

USTRUCT(BlueprintType)
struct FVSFallingMovementAnimSettings : public FTableRowBase
{
	GENERATED_BODY()

	bool IsValid() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (RowType = "/Script/VSPluginsCore.VSAnimSequenceReference"))
	FDataTableRowHandle StartAnimRow;

	/**
	 * Played when the start animation's lift process has finished but the movement is still lifting.
	 * If no valid animation here, a named period in the start animation could be swinging.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimSequenceBase> StartLoopAnim;

	/**
	 * Played when the movement reaches the apex.
	 * If no valid animation here, apex anim could be the time after a mark in the start animation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimSequenceBase> ApexAnim;

	/**
	 * Played when the apex anim is over (or doesn't exist).
	 * If no valid animation here, a named period in the start animation could be swinging.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimSequenceBase> FallAnim;

	/** Just played after falling state before reaching ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (RowType = "/Script/VSPluginsCore.VSAnimSequenceReference"))
	FDataTableRowHandle LandAnimRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSFallingMovementAnimLimits Limits;
};