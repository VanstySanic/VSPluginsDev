// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VSOnlineServiceSubsystem.generated.h"

struct FVSCreateLobbyParams;
/**
 * 
 */
UCLASS()
class VSONLINESERVICES_API UVSOnlineServiceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Online Services|Auth", meta = (AutoCreateRefTerm = "Params"))
	void RegisterAccount();
	
	UFUNCTION(BlueprintCallable, Category = "Online Services|Lobby", meta = (AutoCreateRefTerm = "Params"))
	void CreateLobby(const FVSCreateLobbyParams& Params);

private:
	
};
