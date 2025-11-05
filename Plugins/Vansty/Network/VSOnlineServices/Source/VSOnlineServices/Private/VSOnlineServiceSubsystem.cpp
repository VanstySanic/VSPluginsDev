// Copyright VanstySanic. All Rights Reserved.

#include "VSOnlineServiceSubsystem.h"
#include "Online/Lobbies.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Online/OnlineServices.h"
#include "Types/VSOnlineServiceLobbyTypes.h"

using namespace UE::Online;

void UVSOnlineServiceSubsystem::RegisterAccount()
{
	
}

void UVSOnlineServiceSubsystem::CreateLobby(const FVSCreateLobbyParams& Params)
{
	IOnlineServicesPtr OnlineServices = GetServices();
	check(OnlineServices);
	ILobbiesPtr LobbyInterface = OnlineServices->GetLobbiesInterface();
	check(LobbyInterface);

	FCreateLobby::Params CreateLobbyParams = Params.ToParams();
	LobbyInterface->CreateLobby(MoveTemp(CreateLobbyParams));
}

// IOnlineServicesPtr OnlineServices = GetServices();
// check(OnlineServices);
// ISessionsPtr SessionsInterface = OnlineServices->GetSessionsInterface();
// check(SessionsInterface);
//
// /** Init base create params. */
// FCreateSession::Params CreateSessionParams;
// CreateSessionParams.LocalAccountId = FAccountId(EOnlineServices::Epic, AccountID);
// CreateSessionParams.SessionName = SessionLocalUniqueName;
// CreateSessionParams.bIsLANSession = bIsLANSession;
// CreateSessionParams.SessionSettings.SchemaName = FName("DefaultSchema");
// CreateSessionParams.SessionSettings.NumMaxConnections = NumMaxConnections;
//
// /** Modify custom settings. */
// TMap<FSchemaAttributeId, FCustomSessionSetting> ModifiedCustomSettings;
// for (const TPair<FName, FString>& CustomSetting : CustomSettings)
// {
// 	FCustomSessionSetting CustomSessionSetting;
// 	CustomSessionSetting.Data = FSchemaVariant(CustomSetting.Value);
// 	CustomSessionSetting.Visibility = ESchemaAttributeVisibility::Public;
// 	CustomSessionSetting.ID = INDEX_NONE;
// 	ModifiedCustomSettings.Add(CustomSetting.Key, CustomSessionSetting);
// }
// CreateSessionParams.SessionSettings.CustomSettings = ModifiedCustomSettings;
//
// TOnlineAsyncOpHandle<FCreateSession> CreateSessionResult = SessionsInterface->CreateSession(MoveTemp(CreateSessionParams));
// CreateSessionResult.OnComplete([](const TOnlineResult<FCreateSession>& Result)
// {
// 	if (Result.IsOk())
// 	{
// 		UE_LOG(LogGameSession, Log, TEXT("%s"), TEXT("Session created."));
// 	}
// 	else if (Result.IsError())
// 	{
// 		UE_LOG(LogGameSession, Warning, TEXT("%s"), *ToLogString(Result.GetErrorValue()));
// 	}
// });