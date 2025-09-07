// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSOnlineSessionTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VSOnlineSessionSubsystem.generated.h"

class IOnlineSession;

/**
 * 
 */
UCLASS()
class VSONLINESYSTEM_API UVSOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnlineSessionSignature, FName, SessionName);

public:
	UVSOnlineSessionSubsystem();

	static UVSOnlineSessionSubsystem* Get(UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void CreateOnlineSession(UObject* WorldContext, const FVSOnlineSessionSettings& OnlineSessionSettings = FVSOnlineSessionSettings());

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void JoinOnlineSession(UObject* WorldContext);
	
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnlineSessionSignature OnSessionCreated;
	
private:

};
