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
		Validate,
		Apply,
		Confirm,
		Save
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