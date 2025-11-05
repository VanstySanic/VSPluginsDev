// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSplitScreenTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VSSplitScreenSubsystemGI.generated.h"

/**
 * 
 */
UCLASS()
class VSSPLITSCREEN_API UVSSplitScreenSubsystemGI : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UVSSplitScreenSubsystemGI();

public:
	static UVSSplitScreenSubsystemGI* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "SplitScreen")
	void SetPlayer(int32 PlayerID, EVSSplitScreenPlayer::Type Player);

	UFUNCTION(BlueprintCallable, Category = "SplitScreen")
	EVSSplitScreenPlayer::Type GetPlayerFormID(int32 ID);
	
	UFUNCTION(BlueprintCallable, Category = "SplitScreen")
	int32 GetIDFormPlayer(EVSSplitScreenPlayer::Type Player);
	
protected:
	

private:
	TMap<int32, EVSSplitScreenPlayer::Type> IdentifiedPlayers;
	TMap<EVSSplitScreenPlayer::Type, int32> PlayeredIds;
};
