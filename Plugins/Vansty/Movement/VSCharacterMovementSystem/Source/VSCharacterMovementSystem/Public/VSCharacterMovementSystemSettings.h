// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "VSCharacterMovementSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "Game", DefaultConfig)
class VSCHARACTERMOVEMENTSYSTEM_API UVSCharacterMovementSystemSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UDeveloperSettings Interface
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif
	//~ End UDeveloperSettings Interface

	static const UVSCharacterMovementSystemSettings* Get();
	
	UFUNCTION(BlueprintPure, Category = "Movement")
	static const UVSCharacterMovementSystemSettings* GetCharacterMovementSystemSettingsVS();
	
public:
	/** Define and relate your custom movement mode with gameplay tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Movement")
	TMap<uint8, FGameplayTag> CustomMovementModeTags;
};
