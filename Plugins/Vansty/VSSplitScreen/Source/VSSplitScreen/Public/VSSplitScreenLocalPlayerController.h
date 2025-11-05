// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VSSplitScreenLocalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VSSPLITSCREEN_API AVSSplitScreenLocalPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:
	virtual void InitPlayerState() override;
};
