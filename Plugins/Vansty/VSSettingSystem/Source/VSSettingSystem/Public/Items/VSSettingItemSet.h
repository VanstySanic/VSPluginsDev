// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSSettingSystemTypes.h"
#include "UObject/Object.h"
#include "VSSettingItemSet.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class VSSETTINGSYSTEM_API UVSSettingItemSet : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<UVSSettingItemBase*> GetSettingItems() const { return SettingItems; }

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "Settings")
	TArray<TObjectPtr<UVSSettingItemBase>> SettingItems;
};
