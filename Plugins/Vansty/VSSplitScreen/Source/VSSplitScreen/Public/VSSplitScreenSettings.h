// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VSSplitScreenSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class VSSPLITSCREEN_API UVSSplitScreenSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static const UVSSplitScreenSettings*Get();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Split Screen")
	TSoftClassPtr<APlayerController> SplitScreenLocalPlayerControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Split Screen")
	TArray<FName> SplitScreenMapNames;
};
