// Copyright VanstySanic. All Rights Reserved.

#include "Tasks/VSOnlineServiceTask_RegisterExternalAccount.h"

#include "Online/Auth.h"
#include "Online/OnlineServices.h"

using namespace UE::Online;

UVSOnlineServiceTask_RegisterExternalAccount::UVSOnlineServiceTask_RegisterExternalAccount(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IOnlineServicesPtr OnlineServices = GetServices();
	check(OnlineServices);
	IAuthPtr AuthInterface = OnlineServices->GetAuthInterface();

	
}