// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VSKeyIconConfig.generated.h"

/**
 * Data asset that maps input keys to key-icon textures.
 *
 * Config assets can reference other configs as fallbacks, allowing layered
 * lookup (for example: platform-specific config -> shared default config).
 */
UCLASS()
class VSWIDGETCORE_API UVSKeyIconConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Finds icon by key in this config and referenced fallback configs. */
	UFUNCTION(BlueprintCallable, Category = "Key Icon")
	UTexture2D* GetIconByKey(const FKey& Key);

protected:
	void GetAllKeyIconConfigsRecursive(TArray<UVSKeyIconConfig*>& OutConfigs) const;
	UTexture2D* GetKeyIconSelf(const FKey& Key) const;
	
protected:
	/** Icons keyed by input key. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Icon")
	TMap<FKey, TObjectPtr<UTexture2D>> KeyedIcons;

	/** Referenced fallback configs used as additional source. */
	UPROPERTY(EditAnywhere, Category = "Key Icon")
	TArray<TObjectPtr<UVSKeyIconConfig>> KeyIconConfigs;
};
