// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
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

USTRUCT(BlueprintType)
struct FVSAlphaDurationBlendArgs
{
	GENERATED_BODY()
	
	/**
	 * Duration for the process that not includes the blend-in and blend-out process.
	 * That is, the duration when the alpha is at 1.f.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlphaBlendArgs AlphaBlendInArgs;

	/** This will be reversed during process. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlphaBlendArgs AlphaBlendOutArgs;
};

/**
 * Performs an alpha blend with duration.
 * This will cause the alpha changing from 0.f to 1.f, and then reverse it.
 */
USTRUCT(BlueprintType)
struct FVSAlphaDurationBlend
{
	GENERATED_BODY()

	FVSAlphaDurationBlend(const FVSAlphaDurationBlendArgs& Args = FVSAlphaDurationBlendArgs())
		: AlphaDurationBlendArgs(Args)
	{
	}

	VSPLUGINSCORE_API void Update(float InDeltaTime);
	VSPLUGINSCORE_API float GetAlpha() const { return Alpha; }
	VSPLUGINSCORE_API bool HasFinished() const;
	VSPLUGINSCORE_API void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSAlphaDurationBlendArgs AlphaDurationBlendArgs;

private:
	float Alpha = 0.f;
	float UpdatedTime = 0.f;
};
