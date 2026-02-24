// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VSKeyIconConfig.generated.h"

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSKeyIconConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Key Icon")
	UTexture2D* GetIconByKey(const FKey& Key);

protected:
	void GetAllKeyIconConfigsRecursive(TArray<UVSKeyIconConfig*>& OutConfigs) const;
	UTexture2D* GetKeyIconSelf(const FKey& Key) const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key Icon")
	TMap<FKey, TObjectPtr<UTexture2D>> KeyedIcons;

	UPROPERTY(EditAnywhere, Category = "Key Icon")
	TArray<TObjectPtr<UVSKeyIconConfig>> KeyIconConfigs;
};
