// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "UObject/Object.h"
#include "VSOnlineSessionTypes.generated.h"

USTRUCT(BlueprintType)
struct FVSOnlineSessionSettings
{
	GENERATED_BODY()

	FVSOnlineSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings = FOnlineSessionSettings());

	FOnlineSessionSettings ToOnlineSessionSettings() const;
	
public:
	/** The number of publicly available connections advertised */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPublicConnections;
	
	/** The number of connections that are private (invite/password) only */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPrivateConnections;
	
	/** Whether this match is publicly advertised on the online service */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldAdvertise;
	
	/** Whether joining in progress is allowed or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowJoinInProgress;
	
	/** This game will be lan only and not be visible to external players */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLANMatch;
	
	/** Whether the server is dedicated or player hosted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDedicated;
	
	/** Whether the match should gather stats or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsesStats;
	
	/** Whether the match allows invitations for this session or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowInvites;
	
	/** Whether to display user presence information or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsesPresence;
	
	/** Whether joining via player presence is allowed or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowJoinViaPresence;
	
	/** Whether joining via player presence is allowed for friends only or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowJoinViaPresenceFriendsOnly;
	
	/** Whether the server employs anti-cheat (punkbuster, vac, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAntiCheatProtected;
	
	/** Whether to prefer lobbies APIs if the platform supports them */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseLobbiesIfAvailable;
	
	/** Whether to create (and auto join) a voice chat room for the lobby, if the platform supports it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseLobbiesVoiceChatIfAvailable;
	
	/** Manual override for the Session Id instead of having one assigned by the backend. Its size may be restricted depending on the platform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SessionIdOverride;

	/** Used to keep different builds from seeing each other during searches */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BuildUniqueId;
	
	/** Array of custom session settings */
	FSessionSettings Settings;

	/** Map of custom settings per session member (Not currently used by every OSS) */
	TUniqueNetIdMap<FSessionSettings> MemberSettings;
};
