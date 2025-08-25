// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_ScalabilityQualityLevel.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingItem_ScalabilityQualityLevel : public UVSSettingItemBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualityLevel(int32 InQualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetQualityLevel() const { return QualityLevel; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetCurrentQualityLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetDefaultQualityLevel() const;

private:
	int32 QualityLevel = 0;
};
