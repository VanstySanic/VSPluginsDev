// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "VSSettingSystemTypes.generated.h"

class UVSSettingItemBase;

USTRUCT(BlueprintType)
struct FVSSettingItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::FromString("None");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SpecifyTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CategoryTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer ItemTags;
};

UENUM(BlueprintType)
namespace EVSSettingItemAction
{
	enum Type
	{
		None			UMETA(Hidden),
		Load,
		SetToDefault,
		SetToCurrent,
		SetToLastConfirmed,
		Validate,
		Apply,
		Confirm,
		Save
	};
}

UENUM(BlueprintType)
namespace EVSSettingItemValueSource
{
	enum Type
	{
		None		UMETA(Hidden),

		Default,
		
		/** The current value set on your machine. This could be different form the setting system value.*/
		Current,
		
		/** Value in the setting system. This could be different form the current value. */
		Settings,

		LastConfirmed
	};
}