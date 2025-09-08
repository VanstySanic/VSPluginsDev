// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundMix.h"
#include "VSSettingSystemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
	static FSoundClassAdjuster GetActiveSoundClassAdjuster(USoundMix* SoundMix, USoundClass* SoundClass);
};
