// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSOnlineServiceSessionTypes.h"

using namespace UE::Online;

FVSCreateSessionSessionParams::FVSCreateSessionSessionParams(const FCreateSession::Params& InParams)
{
	/** Params. */
	LocalAccountId = InParams.LocalAccountId.GetHandle();
	SessionName = InParams.SessionName;
	SessionIdOverride = InParams.SessionIdOverride;
	bIsLANSession = InParams.bIsLANSession;
	NumMaxConnections = InParams.SessionSettings.NumMaxConnections;
	bPresenceEnabled = InParams.bPresenceEnabled;
	bAllowSanctionedPlayers = InParams.bAllowSanctionedPlayers;
	bAntiCheatProtected = InParams.bAntiCheatProtected;
	JoinPolicy = (EVSSessionJoinPolicy::Type)(uint8)InParams.SessionSettings.JoinPolicy;
	SchemaName = InParams.SessionSettings.SchemaName;

	for (const TPair<FName, FCustomSessionSetting>& CustomSetting : InParams.SessionSettings.CustomSettings)
	{
		CustomSettings.Emplace(CustomSetting.Key, CustomSetting.Value.Data.GetString());
	}
}

FCreateSession::Params FVSCreateSessionSessionParams::ToParams() const
{
	FCreateSession::Params OutParams;

	OutParams.LocalAccountId = FAccountId(EOnlineServices::Epic, LocalAccountId);
	OutParams.SessionName = SessionName;
	OutParams.SessionIdOverride = SessionIdOverride;
	OutParams.bIsLANSession = bIsLANSession;
	OutParams.SessionSettings.NumMaxConnections = NumMaxConnections;
	OutParams.bPresenceEnabled = bPresenceEnabled;
	OutParams.bAllowSanctionedPlayers = bAllowSanctionedPlayers;
	OutParams.bAntiCheatProtected = bAntiCheatProtected;
	OutParams.SessionSettings.JoinPolicy = (ESessionJoinPolicy)(uint8)JoinPolicy;
	OutParams.SessionSettings.SchemaName = SchemaName;
	
	/** Modify custom settings. */
	TMap<FSchemaAttributeId, FCustomSessionSetting> ModifiedCustomSettings;
	for (const TPair<FName, FString>& CustomSetting : CustomSettings)
	{
		FCustomSessionSetting CustomSessionSetting;
		CustomSessionSetting.Data = FSchemaVariant(CustomSetting.Value);
		CustomSessionSetting.Visibility = ESchemaAttributeVisibility::Public;
		CustomSessionSetting.ID = INDEX_NONE;
		ModifiedCustomSettings.Add(CustomSetting.Key, CustomSessionSetting);
	}
	OutParams.SessionSettings.CustomSettings = ModifiedCustomSettings;
	
	return OutParams;
}
