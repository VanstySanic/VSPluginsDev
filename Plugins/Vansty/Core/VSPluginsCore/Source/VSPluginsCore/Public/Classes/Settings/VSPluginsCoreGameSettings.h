// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "VSPluginsCoreGameSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class VSPLUGINSCORE_API UVSPluginsCoreGameSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	//~ End UDeveloperSettings Interface
	
	static const UVSPluginsCoreGameSettings* Get();
	
	UFUNCTION(BlueprintPure, Category = "Settings")
	static const UVSPluginsCoreGameSettings* GetVSPluginsCoreGameSettings();

public:
	/**
	 * Gameplay tags whose initial counts should be captured on the server and replicated once to autonomous proxies.
	 * Allow implicit matching.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags")
	FGameplayTagContainer InitialAutonomousReplicatedGameplayTags;

	/**
	 * Gameplay tags whose initial counts should be captured on the server and replicated once to simulated proxies.
	 * Allow implicit matching.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags")
	FGameplayTagContainer InitialSimulationReplicatedGameplayTags;

	/**
	 * Gameplay tag counts on the ASC to listen to for AnyCountChange events when using the ASC as the tag source.
	 * Explicit tags only.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags")
	FGameplayTagContainer AbilitySystemListeningCountedTags;

	/** Gameplay event tags to listen from ASC. Allow implicit matching. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags")
	FGameplayTagContainer AbilitySystemListeningGameplayEventTags;

	/** Gameplay event tags allowed to send to ASC. Allow implicit matching. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "GameplayTags")
	FGameplayTagContainer AbilitySystemSendingGameplayEventTags;
};
