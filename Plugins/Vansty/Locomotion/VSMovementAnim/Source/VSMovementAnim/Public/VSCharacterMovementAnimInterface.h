// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSCharacterMovementAnimInterface.generated.h"

class UVSCharacterMovementAnimFeatureAgent;

UINTERFACE()
class UVSCharacterMovementAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSMOVEMENTANIM_API IVSCharacterMovementAnimInterface
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	UVSCharacterMovementAnimFeatureAgent* GetCharacterMovementAnimFeatureAgent() const;
};
