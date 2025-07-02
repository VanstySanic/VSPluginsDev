// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSCharacterMovementInterface.generated.h"

class UVSCharacterMovementFeatureAgent;

UINTERFACE()
class UVSCharacterMovementInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSMOVEMENTSYSTEM_API IVSCharacterMovementInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	UVSCharacterMovementFeatureAgent* GetMovementAgentFeature() const;
};
