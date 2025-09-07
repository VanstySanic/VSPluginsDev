// Fill out your copyright notice in the Description page of Project Settings.

#include "VSOnlineSessionSubsystem.h"
#include "Online.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "Kismet/GameplayStatics.h"

UVSOnlineSessionSubsystem::UVSOnlineSessionSubsystem()
{
	
}

UVSOnlineSessionSubsystem* UVSOnlineSessionSubsystem::Get(UObject* WorldContext)
{
	if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext))
	{
		return GameInstance->GetSubsystem<UVSOnlineSessionSubsystem>();
	}
	return nullptr;
}


void UVSOnlineSessionSubsystem::CreateOnlineSession(UObject* WorldContext, const FVSOnlineSessionSettings& OnlineSessionSettings)
{
	if (!WorldContext || !WorldContext->GetWorld()) return;
	
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(nullptr, NAME_None);
	check(OnlineSubsystem);

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	check(SessionInterface.IsValid());
	
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	if (APlayerController* PlayerController = WorldContext->GetWorld()->GetFirstPlayerController())
	{
		
	}
}

void UVSOnlineSessionSubsystem::JoinOnlineSession(UObject* WorldContext)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(nullptr, NAME_None);
	check(OnlineSubsystem);

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	check(SessionInterface.IsValid());
}

