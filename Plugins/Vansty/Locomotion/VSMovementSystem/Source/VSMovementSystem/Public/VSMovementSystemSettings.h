// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "Types/VSChrMovOrientationTypes.h"
#include "VSMovementSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "Game", DefaultConfig)
class VSMOVEMENTSYSTEM_API UVSMovementSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif

	static const UVSMovementSystemSettings* Get() ;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	static FGameplayTag GetTagFromMovementMode(const EMovementMode InMovementMode, const uint8 InCustomMode = 0);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	static bool GetMovementModeFromTag(const FGameplayTag& InTag, TEnumAsByte<EMovementMode>& OutMovementMode, uint8& OutCustomMode);
	
public:
	/** Define and relate your custom movement mod with gameplay tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Mode", Config)
	TMap<uint8, FGameplayTag> CustomMovementModeTags;

	/**
	 * X, Y, Z means forward, right and up.
	 * This is component space. The animation space's Y/X axes are the component space's X/Y.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Name", Config)
	TMap<TEnumAsByte<EAxis::Type>, FName> AnimMovementCurveNames =
	{
		{ EAxis::X, FName("Movement_Forward") },
		{ EAxis::Y, FName("Movement_Right") },
		{ EAxis::Z, FName("Movement_Up") },
	};
};
