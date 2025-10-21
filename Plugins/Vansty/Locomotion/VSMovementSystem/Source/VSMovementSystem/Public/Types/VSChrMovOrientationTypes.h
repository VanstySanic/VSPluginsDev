// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSGameplayTypes.h"
#include "UObject/Object.h"
#include "VSChrMovOrientationTypes.generated.h"

struct FGameplayTag;

UENUM(BlueprintType)
namespace EVSMovementRelatedOrientationType
{
	enum Type 
	{
		None,
		Self,
		Velocity,
		Input,
		Control,
		Aim,
		Custom,
	};
}

UENUM(BlueprintType)
namespace EVSMovementOrientationAimTargetType
{
	enum Type
	{
		None,
		Point,
		Direction,
		Actor,
		Component,
		Socket
	};
}

USTRUCT(BlueprintType)
struct FVSMovementOrientationDynamicData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SelfRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MovementInput = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator ControlRotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FVSMovementOrientationAimData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Point = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> Component = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> SocketComponent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FName SocketName = NAME_None;
};

USTRUCT(BlueprintType)
struct FVSMovementOrientationEvaluateType
{
	GENERATED_BODY()

	FVSMovementOrientationEvaluateType(EVSMovementRelatedOrientationType::Type OrientationType = EVSMovementRelatedOrientationType::None, EVSMovementOrientationAimTargetType::Type AimTargetType = EVSMovementOrientationAimTargetType::None)
		: OrientationType(OrientationType)
		, AimTargetType(AimTargetType)
	{
	}

	friend uint32 GetTypeHash(const FVSMovementOrientationEvaluateType& Type)
	{
		return HashCombine(GetTypeHash(Type.OrientationType), GetTypeHash(Type.AimTargetType));
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSMovementRelatedOrientationType::Type> OrientationType = EVSMovementRelatedOrientationType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSMovementOrientationAimTargetType::Type> AimTargetType = EVSMovementOrientationAimTargetType::None;
};

USTRUCT(BlueprintType)
struct FVSMovementOrientationEvaluateParams
{
	GENERATED_BODY()

	FVSMovementOrientationEvaluateParams(const FVSMovementOrientationEvaluateType& EvaluateTypes = FVSMovementOrientationEvaluateType())
		: EvaluateType(EvaluateTypes)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSMovementOrientationEvaluateType EvaluateType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EVSMovementRelatedOrientationType::Type>> OverridenRotationTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSMovementOrientationDynamicData DynamicDataOverride;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EVSMovementOrientationAimTargetType::Type>> OverridenAimTargetTypes;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSMovementOrientationAimData AimDataOverride;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CustomRotation = FRotator::ZeroRotator;

	/** If true, the movement will be adjusted if moving against wall. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMovementAgainstWallAdjustment2D = true;
	
	/** If true, return the gravity space 2d rotation instead of the 3d rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReturnRotationInSpace2D = true;
};

USTRUCT(BlueprintType)
struct FVSOrientationControlSettings
{
	GENERATED_BODY()

	friend uint32 GetTypeHash(const FVSOrientationControlSettings& Settings)
	{
		const uint32 HashA = HashCombine(GetTypeHash(Settings.EvaluateType), GetTypeHash(Settings.EvaluateType));
		const uint32 HashB = HashCombine(GetTypeHash(Settings.bControlOnly2D), GetTypeHash(Settings.OrientationLagSpeed));
		return HashCombine(HashA, HashB);
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVSMovementOrientationEvaluateType EvaluateType = FVSMovementOrientationEvaluateType(EVSMovementRelatedOrientationType::None);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bControlOnly2D = true;
	
	/** Lag the moving orientation 2D to the desired. 0.f means no lag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OrientationLagSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OrientationLagMaxTimeSubstepping = 0.0166667f;
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

