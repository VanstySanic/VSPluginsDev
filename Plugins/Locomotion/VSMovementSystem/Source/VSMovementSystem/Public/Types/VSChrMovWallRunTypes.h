// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSChrMovWallRunTypes.generated.h"

USTRUCT(BlueprintType)
struct FVSWallRunLimits
{
	GENERATED_BODY()

	
};

USTRUCT(BlueprintType)
struct FVSWallRunSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FDataTableRowHandle StartAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequenceBase> CycleAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FDataTableRowHandle EndAnim;

	/** Used in cycle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWallRunSpeed = 512.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSWallRunLimits Limits;
};
