// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "VSPluginsCoreSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class VSPLUGINSCORE_API UVSPluginsCoreSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	static const UVSPluginsCoreSettings* Get();
	
	UFUNCTION(BlueprintPure, Category = "Settings")
	static const UVSPluginsCoreSettings* GetPluginsCoreSettings_VS();

	virtual FName GetCategoryName() const override;
	

public:
	/** Explicit tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags|Feature")
	FGameplayTagContainer InitialAutonomousReplicatedGameplayTags;

	/** Explicit tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags|Feature")
	FGameplayTagContainer InitialSimulationReplicatedGameplayTags;

	/** Explicit tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags|Feature")
	FGameplayTagContainer AbilitySystemComponentListeningTags;
};
