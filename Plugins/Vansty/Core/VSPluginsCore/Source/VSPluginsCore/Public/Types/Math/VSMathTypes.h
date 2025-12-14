// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSMathTypes.generated.h"

/** Bitmask defining which FRotator axes are affected. */
UENUM(BlueprintType)
namespace EVSRotatorAxes
{
	enum Type : uint8
	{
		None = 0,

		Roll  = 1 << 0,
		Pitch = 1 << 1,
		Yaw   = 1 << 2,

		RollPitch = Roll + Pitch,
		RollYaw   = Roll + Yaw,
		PitchYaw  = Pitch + Yaw,

		RollPitchYaw = Roll + Pitch + Yaw,
	};
}

/** Bitmask defining which FVector axes are affected. */
UENUM(BlueprintType)
namespace EVSVectorAxes
{
	enum Type : uint8
	{
		None = 0,

		X = 1 << 0,
		Y = 1 << 1,
		Z = 1 << 2,

		XY = X + Y,
		XZ = X + Z,
		YZ = Y + Z,

		XYZ = X + Y + Z,
	};
}

/** Defines axis swizzle order for vector component remapping. */
UENUM(BlueprintType)
namespace EVSAxesSwizzle
{
	enum Type : uint8
	{
		/** No axis reordering. */
		XYZ,

		/** Swaps X and Y axes. */
		YXZ,

		/** Swaps X and Z axes. */
		ZYX,

		/** Swaps Y and Z axes. */
		XZY,

		/** Reorders axes with Y as the first component. */
		YZX,

		/** Reorders axes with Z as the first component. */
		ZXY,
	};
}

/** Defines which axes of a transform are affected by an operation. */
USTRUCT(BlueprintType)
struct FVSTransformAxes
{
	GENERATED_BODY()

	/** Affected rotation axes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSRotatorAxes::Type> RotatorAxes = EVSRotatorAxes::RollPitchYaw;

	/** Affected translation axes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSVectorAxes::Type> TranslationAxes = EVSVectorAxes::XYZ;

	/** Affected scale axes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSVectorAxes::Type> ScaleAxes = EVSVectorAxes::XYZ;
};

/** Defines rotation direction relative to a reference orientation. */
UENUM(BlueprintType)
namespace EVSRotationDirection
{
	enum Type
	{
		None,
		Clockwise,
		CounterClockwise
	};
}