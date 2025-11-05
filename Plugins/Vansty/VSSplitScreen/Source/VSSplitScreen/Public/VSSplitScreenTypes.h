// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSSplitScreenTypes.generated.h"

class UCameraComponent;

UENUM(BlueprintType)
namespace EVSSplitScreenPlayer
{
	enum Type
	{
		None,
		PlayerOne,
		PlayerTwo,
	};
}

USTRUCT(BlueprintType)
struct FVSSplitScreenPlayerData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ULocalPlayer> LocalPlayer;

	/**
	 * The local player controller that handles the view and widget.
	 * This is not the common player controller.
	 * For locally controlled player, this is the local player controller.
	 * For remote player, this is dynamically spawned in local client.
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerController> LocalController;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APlayerState> PlayerState;
};


