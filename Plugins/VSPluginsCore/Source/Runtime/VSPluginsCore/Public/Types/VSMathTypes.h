// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSMathTypes.generated.h"

UENUM(BlueprintType)
namespace EVSRotatorAxes
{
	enum Type : uint8
	{
		None			 = 0,							
		
		Roll			 = 1 << 0,
		Pitch			 = 1 << 1,
		Yaw				 = 1 << 2,

		RollPitch		 = Roll + Pitch,
		RollYaw			 = Roll + Yaw,
		PitchYaw		 = Pitch + Yaw,
		
		RollPitchYaw	 = Roll + Pitch + Yaw,
	};
}

UENUM(BlueprintType)
namespace EVSVectorAxes
{
	enum Type : uint8
	{
		None	 = 0,							
		
		X		 = 1 << 0,
		Y		 = 1 << 1,
		Z		 = 1 << 2,

		XY		 = X + Y,
		XZ		 = X + Z,
		YZ		 = Y + Z,
		
		XYZ		 = X + Y + Z,
	};
}

USTRUCT(BlueprintType)
struct FVSTransformAxes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSRotatorAxes::Type> RotatorAxes = EVSRotatorAxes::RollPitchYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSVectorAxes::Type> TranslationAxes = EVSVectorAxes::XYZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSVectorAxes::Type> ScaleAxes = EVSVectorAxes::XYZ;
};
