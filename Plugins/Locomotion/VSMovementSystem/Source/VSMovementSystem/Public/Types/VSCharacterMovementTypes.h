// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSCharacterMovementTypes.generated.h"

class UCharacterMovementComponent;

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