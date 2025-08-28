// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSSettingSystemTypes.generated.h"

class UVSSettingItemBase;

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
namespace EVSSettingItemValueType
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

USTRUCT(BlueprintType)
struct FVSSettingItemGroup
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	/** Only exist in editor do distinguish between groups. */
	UPROPERTY(EditAnywhere)
	FName Name;
#endif
	
	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UVSSettingItemBase>> SettingItems;
};