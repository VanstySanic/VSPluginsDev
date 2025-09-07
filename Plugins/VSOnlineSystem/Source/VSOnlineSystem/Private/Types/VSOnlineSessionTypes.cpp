// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/VSOnlineSessionTypes.h"

FVSOnlineSessionSettings::FVSOnlineSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings)
	: NumPublicConnections(OnlineSessionSettings.NumPublicConnections)
	, NumPrivateConnections(OnlineSessionSettings.NumPublicConnections)
	, bShouldAdvertise(OnlineSessionSettings.bShouldAdvertise)
	, bAllowJoinInProgress(OnlineSessionSettings.bAllowJoinInProgress)
	, bIsLANMatch(OnlineSessionSettings.bIsLANMatch)
	, bIsDedicated(OnlineSessionSettings.bIsDedicated)
	, bUsesStats(OnlineSessionSettings.bUsesStats)
	, bAllowInvites(OnlineSessionSettings.bAllowInvites)
	, bUsesPresence(OnlineSessionSettings.bUsesPresence)
	, bAllowJoinViaPresence(OnlineSessionSettings.bAllowJoinViaPresence)
	, bAllowJoinViaPresenceFriendsOnly(OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly)
	, bAntiCheatProtected(OnlineSessionSettings.bAntiCheatProtected)
	, bUseLobbiesIfAvailable(OnlineSessionSettings.bUseLobbiesIfAvailable)
	, bUseLobbiesVoiceChatIfAvailable(OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable)
	, BuildUniqueId(OnlineSessionSettings.BuildUniqueId)
{
}

FOnlineSessionSettings FVSOnlineSessionSettings::ToOnlineSessionSettings() const
{
	FOnlineSessionSettings OnlineSessionSettings;
	OnlineSessionSettings.NumPublicConnections = NumPublicConnections;
	OnlineSessionSettings.NumPrivateConnections = NumPrivateConnections;
	OnlineSessionSettings.bShouldAdvertise = bShouldAdvertise;
	OnlineSessionSettings.bAllowJoinInProgress = bAllowJoinInProgress;
	OnlineSessionSettings.bIsLANMatch = bIsLANMatch;
	OnlineSessionSettings.bIsDedicated = bIsDedicated;
	OnlineSessionSettings.bUsesStats = bUsesStats;
	OnlineSessionSettings.bAllowInvites = bAllowInvites;
	OnlineSessionSettings.bUsesPresence = bUsesPresence;
	OnlineSessionSettings.bAllowJoinViaPresence = bAllowJoinViaPresence;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	OnlineSessionSettings.bAntiCheatProtected = bAntiCheatProtected;
	OnlineSessionSettings.bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = bUseLobbiesVoiceChatIfAvailable;
	OnlineSessionSettings.BuildUniqueId = BuildUniqueId;
	return OnlineSessionSettings;
}
