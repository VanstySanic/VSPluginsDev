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
	/** Gameplay tags whose initial explicit counts should be captured on the server and replicated once to autonomous proxies. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags|Feature")
	FGameplayTagContainer InitialAutonomousReplicatedGameplayTags;

	/** Gameplay tags whose initial explicit counts should be captured on the server and replicated once to simulated proxies. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags|Feature")
	FGameplayTagContainer InitialSimulationReplicatedGameplayTags;

	/** Gameplay tags on the AbilitySystemComponent to listen to for AnyCountChange events when using the ASC as the tag source. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags|Feature")
	FGameplayTagContainer AbilitySystemComponentListeningTags;
};
