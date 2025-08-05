// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSMathTypes.h"
#include "VSCameraTypes.generated.h"

UENUM(BlueprintType)
namespace EVSCameraRelatedTransformType
{
	enum Type
	{
		None				UMETA(Hidden),
		
		World,

		/** The camera's local transform. */
		Local,

		/** The camera's view transform, applying local and view offset. */
		LocalView,

		/** The camera's transform in the view data. */
		Data,

		/** The camera's view transform in the view data, applying local and view offset. */
		DataView,
		
		AttachedActor,
		AttachedComponent,
		AttachedSocket,

		/** Assign a custom space by your self. */
		Custom
	};
}

USTRUCT(BlueprintType)
struct FVSCameraVectorModifySettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSCameraRelatedTransformType::Type> ModifySpaceType = EVSCameraRelatedTransformType::World;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModifySpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FTransform CustomModifySpace = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSVectorAxes::Type> AxesToModify = EVSVectorAxes::XYZ;
};

USTRUCT(BlueprintType)
struct FVSCameraRotatorModifySettings
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSCameraRelatedTransformType::Type> ModifySpaceType = EVSCameraRelatedTransformType::World;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModifySpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FRotator CustomModifySpace = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSRotatorAxes::Type> AxesToModify = EVSRotatorAxes::RollPitchYaw;;
};

USTRUCT(BlueprintType)
struct FVSCameraTransformModifySettings
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSCameraRelatedTransformType::Type> ModifySpaceType = EVSCameraRelatedTransformType::World;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModifySpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FTransform CustomModifySpace = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSTransformAxes AxesToModify;
};

USTRUCT(BlueprintType)
struct FVSCameraVectorLagSettings
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSCameraRelatedTransformType::Type> LagSpaceType = EVSCameraRelatedTransformType::World;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "LagSpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FTransform CustomLagSpace = FTransform::Identity;
	
	/** Lag space of the follow transform movement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LagSpeedVector = FVector(10.0);

	/**
	 * The time sub-stepping in lagging process. This will make the lag look smoother, but may do some cost.
	 * Set to 0.f or negative to disable sub-stepping in lag process.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLagTimeSubStepping = 0.0166667f;
};

USTRUCT(BlueprintType)
struct FVSCameraRotatorLagSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSCameraRelatedTransformType::Type> LagSpaceType = EVSCameraRelatedTransformType::World;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "LagSpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FRotator CustomLagSpace = FRotator::ZeroRotator;
	
	/** Lag space of the follow transform movement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator LagSpeedRotator = FRotator(10.0);

	/**
	 * The time sub-stepping in lagging process. This will make the lag look smoother, but may do some cost.
	 * Set to 0.f or negative to disable sub-stepping in lag process.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLagTimeSubStepping = 0.0166667f;
};

USTRUCT(BlueprintType)
struct FVSCameraTransformLagSettings
{
	GENERATED_USTRUCT_BODY()

	FVSCameraTransformLagSettings(){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSCameraRelatedTransformType::Type> LagSpaceType = EVSCameraRelatedTransformType::World;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "LagSpaceType == EVSCameraRelatedTransformType::Custom", EditConditionHides))
	FRotator CustomLagSpace = FRotator::ZeroRotator;
	
	/** Lag space of the follow transform movement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform LagSpeedTransform = FTransform(FRotator(10.0), FVector(10.0), FVector(10.0));

	/**
	 * The time sub-stepping in lagging process. This will make the lag look smoother, but may do some cost.
	 * Set to 0.f or negative to disable sub-stepping in lag process.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLagTimeSubStepping = 0.0166667f;
};