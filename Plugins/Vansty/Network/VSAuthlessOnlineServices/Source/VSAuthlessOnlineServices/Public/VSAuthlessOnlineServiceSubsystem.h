// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VSAuthlessOnlineServiceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class VSAUTHLESSONLINESERVICES_API UVSAuthlessOnlineServiceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAuthlessOnlineServiceSignature);

public:
	UFUNCTION(BlueprintCallable, Category = "Online Services", meta = (AutoCreateRefTerm = "CustomSettings"))
	static void Login();

	UFUNCTION(BlueprintCallable, Category = "Online Services", meta = (AutoCreateRefTerm = "CustomSettings"))
	static void CreateAuthlessSession(int32 AccountID, FName SessionLocalUniqueName, bool bIsLANSession, int32 NumMaxConnections, const TMap<FName, FString>& CustomSettings);

protected:
	
};
