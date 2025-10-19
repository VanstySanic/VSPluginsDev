// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/VSGameplayTypes.h"
#include "Types/Animation/VSAnimSequenceReference.h"
#include "UObject/Object.h"
#include "VSChrMovMantleVaultTypes.generated.h"

class UVSCharacterMovementFeatureAgent;

UENUM(BlueprintType)
namespace EVSMantleVaultMovementType
{
	enum Type
	{
		None				= 0							UMETA(Hidden),
		Mantle				= 1 << 0,
		Vault				= 1 << 1,
		MantleOrVault		= Mantle + Vault,
	};
}

/** Defines a mantle / vault animation. */
USTRUCT(BlueprintType)
struct FVSMantleVaultAnimSettings : public FTableRowBase
{
	GENERATED_BODY()
	
	VSMOVEMENTSYSTEM_API bool IsValid(const FName AnimScaleMovementToReachTargetTimeMarkName = FName("GroundPivot"), const FName AnimReachTargetTimeMarkName = FName("ReachTarget"), const FName AnimGroundPivotTimeMarkName = FName("GroundPivot"), const FName AnimVaultOffPlatformTimeMarkName = FName("VaultOffPlatform")) const;

	/** Can be an anim sequence or a montage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "/Script/VSPluginsCore.VSAnimSequenceReference"))
	FDataTableRowHandle AnimRow;
	
	/**
	 * The valid distance range between the root and the wall when the character reaches the wall target.
	 * This is used to enhance the performance but not works as limit.
	 * The distance could be negative.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ReachTargetDistanceToWallRange = FVector2D(36.f, 36.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSMantleVaultMovementType::Type> MovementType = EVSMantleVaultMovementType::None;
};

USTRUCT(BlueprintType)
struct FVSMantleVaultLimits
{
	GENERATED_BODY()

	FVSMantleVaultLimits() { }
	
	VSMOVEMENTSYSTEM_API bool IsValid() const;

	/** Defines how high the character can mantle / vault with the settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D PlatformHeightRange = FVector2D(0.0, 300.0);
	
	/** The character's distance to wall must be in that range.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DistanceToWallRange2D = FVector2D(0.0, 1024.0);
	
	/** The character's speed towards the inner wall normal must be in that range.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpeedTowardsMovementRange2D = FVector2D(0.0, 1024.0);

	/** The character's forward vector should be in that angle against the inner wall normal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FacingMovementMaxAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Velocity2DTowardsMovementMaxAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Input2DTowardsMovementMaxAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequireMovementInput2D = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSSceneComponentQuery TerrainComponentQuery; 

	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery MovementTagQuery;
};

USTRUCT(BlueprintInternalUseOnly)
struct FVSMantleVaultSettings : public FTableRowBase
{
	GENERATED_BODY()
	
	VSMOVEMENTSYSTEM_API bool IsValid() const;
	
	/** The mantle animations in the settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "/Script/VSMovementSystem.VSMantleVaultAnimSettings"))
	TArray<FDataTableRowHandle> AnimRows;
	
	/** Whether the animations in the settings should be randomized during process. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRandomizeAnims = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TraceForwardDistance = 512.f;
	
	/** Defines the valid working condition. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSMantleVaultLimits Limits;

	/** Below 0.f means default half height. Unscaled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsuleHalfHeight = 0.f;
};

/** Snapped params to define a mantle / vault movement and its animation. Replicated. */
USTRUCT(BlueprintType)
struct VSMOVEMENTSYSTEM_API FVSMantleVaultSnappedParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle SettingsRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle AnimSettingsRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSMantleVaultMovementType::Type> MovementType = EVSMantleVaultMovementType::None;

	/** This might be lost because of non-replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor;

	/** This might be lost because of non-replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ComponentName = NAME_None;

	/** Safe during replication. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform StartComponentTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MovementDirection2DRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FrontWallPointRS = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GroundPivotPointRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VaultOffPlatformPointRS = FVector::ZeroVector;
	
	/** Animation curve value, not scaled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimReachTargetDistanceToWall2D = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ServerSideServerStartTime = 0.f;
};

USTRUCT(BlueprintType)
struct FVSMantleVaultCachedParams
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimPlayRate = 1.f;
	
	/** Scaled by character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlatformHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UPrimitiveComponent> Component;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartRootLocationRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ScaleMovementToReachTargetRootLocationRS = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ReachTargetRootLocationRS = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ReachTargetRootLocationScaledRS = FVector::ZeroVector;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AnimScaleMovementToReachTargetCurveValues = FVector2D::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AnimStartMovementCurveValues = FVector2D::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AnimReachTargetMovementValues = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AnimGroundPivotMovementValues = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AnimVaultOffPlatformMovementCurveValues = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClientSideServerStartTime = 0.f;
	
	/** This is for animation, used to restart the anim action. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActionID = INDEX_NONE;
};