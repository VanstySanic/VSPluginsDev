// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSCharacterMovementTypes.generated.h"

class UCharacterMovementComponent;

UENUM(BlueprintType)
namespace EVSMovementRelatedOrientationType
{
	enum Type 
	{
		None				UMETA(Hidden),
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
		None				UMETA(Hidden),
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> Component = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> SocketComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
struct FVSMovementBaseSettings
{
	GENERATED_BODY()

	FVSMovementBaseSettings& operator=(const FVSMovementBaseSettings& Other)
	{
		MaxSpeed = Other.MaxSpeed;
		MaxAcceleration = Other.MaxAcceleration;
		BrakingDeceleration = Other.BrakingDeceleration;
		BrakingFriction = Other.BrakingFriction;
		BrakingFrictionFactor = Other.BrakingFrictionFactor;
		bUseSeparateBrakingFriction = Other.bUseSeparateBrakingFriction;
		return *this;
	}

	bool operator==(const FVSMovementBaseSettings& Other) const
	{
		if (MaxSpeed != Other.MaxSpeed) return false;
		if (MaxAcceleration != Other.MaxAcceleration) return false;
		if (BrakingDeceleration != Other.BrakingDeceleration) return false;
		if (BrakingFriction != Other.BrakingFriction) return false;
		if (BrakingFrictionFactor != Other.BrakingFrictionFactor) return false;
		if (bUseSeparateBrakingFriction != Other.bUseSeparateBrakingFriction) return false;
		return true;
	}

	friend uint32 GetTypeHash(const FVSMovementBaseSettings& Settings)
	{
		const int32 HashA = HashCombine(GetTypeHash(Settings.MaxSpeed), GetTypeHash(Settings.MaxAcceleration));
		const int32 HashB = HashCombine(GetTypeHash(Settings.BrakingDeceleration), GetTypeHash(Settings.BrakingDeceleration));
		const int32 HashC = HashCombine(GetTypeHash(Settings.BrakingFriction), GetTypeHash(Settings.bUseSeparateBrakingFriction));
		const int32 HashD = HashCombine(HashA, HashB);
		return HashCombine(HashC, HashD);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingDeceleration = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFrictionFactor = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFriction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseSeparateBrakingFriction = true;
};

USTRUCT(BlueprintType)
struct FVSChrMovCapsuleHalfHeightAdjustParams
{
	GENERATED_BODY()

	FVSChrMovCapsuleHalfHeightAdjustParams(const float CapsuleHalfHeight = 0.f, const bool bHalfHeightAsDelta = false, const bool bKeepRootPosition = true, const bool bKeepMeshPosition= true)
		: HalfHeight(CapsuleHalfHeight), bHalfHeightAsDelta(bHalfHeightAsDelta), bKeepRootPosition(bKeepRootPosition), bKeepMeshPosition(bKeepMeshPosition)
	{
		
	}
	
	/** Unscaled. Use default value if below zero and is not delta. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HalfHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHalfHeightAsDelta = false;

	/** If true, when adjusting the capsule half height, an offset will be added to the capsule so that the root location will be kept in world. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bKeepRootPosition = true;

	/**
	 * If true, when adjusting the capsule half height, an offset will be added to the mesh so that the mesh location will be kept in world.
	 * Only works when bKeepRootPosition == true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bKeepRootPosition"))
	bool bKeepMeshPosition = true;
};