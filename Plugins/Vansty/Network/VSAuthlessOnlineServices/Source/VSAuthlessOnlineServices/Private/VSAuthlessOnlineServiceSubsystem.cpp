// Copyright VanstySanic. All Rights Reserved.

#include "VSAuthlessOnlineServiceSubsystem.h"
#include "GameFramework/GameSession.h"
#include "Online/Auth.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Online/OnlineServices.h"
#include "Online/Sessions.h"

using namespace UE::Online;

void UVSAuthlessOnlineServiceSubsystem::Login()
{
	IOnlineServicesPtr OnlineServices = GetServices();
	check(OnlineServices);
	

}

void UVSAuthlessOnlineServiceSubsystem::CreateAuthlessSession(int32 AccountID, FName SessionLocalUniqueName, bool bIsLANSession, int32 NumMaxConnections, const TMap<FName, FString>& CustomSettings)
{
	IOnlineServicesPtr OnlineServices = GetServices();
	check(OnlineServices);
	ISessionsPtr SessionsInterface = OnlineServices->GetSessionsInterface();
	check(SessionsInterface);

	/** Init base create params. */
	FCreateSession::Params CreateSessionParams;
	CreateSessionParams.LocalAccountId = FAccountId(EOnlineServices::Epic, AccountID);
	CreateSessionParams.SessionName = SessionLocalUniqueName;
	CreateSessionParams.bIsLANSession = bIsLANSession;
	CreateSessionParams.SessionSettings.SchemaName = FName("DefaultSchema");
	CreateSessionParams.SessionSettings.NumMaxConnections = NumMaxConnections;

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
	CreateSessionParams.SessionSettings.CustomSettings = ModifiedCustomSettings;
	
	
	TOnlineAsyncOpHandle<FCreateSession> CreateSessionResult = SessionsInterface->CreateSession(MoveTemp(CreateSessionParams));
	CreateSessionResult.OnComplete([](const TOnlineResult<FCreateSession>& Result)
	{
		if (Result.IsOk())
		{
			UE_LOG(LogGameSession, Log, TEXT("%s"), TEXT("Session created."));
		}
		else if (Result.IsError())
		{
			UE_LOG(LogGameSession, Warning, TEXT("%s"), *ToLogString(Result.GetErrorValue()));
		}
	});
}
