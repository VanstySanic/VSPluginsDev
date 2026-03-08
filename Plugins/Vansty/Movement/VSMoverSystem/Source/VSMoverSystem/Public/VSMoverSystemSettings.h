// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "VSMoverSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "Game", DefaultConfig)
class VSMOVERSYSTEM_API UVSMoverSystemSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UDeveloperSettings Interface
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif
	//~ End UDeveloperSettings Interface

	static const UVSMoverSystemSettings* Get();
	
	UFUNCTION(BlueprintPure, Category = "Movement")
	static const UVSMoverSystemSettings* GetMoverSystemSettingsVS();
	
public:
	/** Define and relate your custom movement mode with gameplay tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Movement")
	TMap<FName, FGameplayTag> CustomMovementModeTags;
};
