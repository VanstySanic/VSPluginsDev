// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSGameplayTypes.h"

FVSNetMethodExecutionPolicies FVSNetMethodExecutionPolicies::LocalExecution = FVSNetMethodExecutionPolicies();
FVSNetMethodExecutionPolicies FVSNetMethodExecutionPolicies::AutonomousPredictedMulticast = FVSNetMethodExecutionPolicies(
	EVSNetAutonomousMethodExecPolicy::ClientAndServer,
	EVSNetAuthorityMethodExecPolicy::Multicast,
	EVSNetAuthorityMethodExecPolicy::ServerAndSimulated,
	false);
FVSNetMethodExecutionPolicies FVSNetMethodExecutionPolicies::AuthorityMulticast = FVSNetMethodExecutionPolicies(
	EVSNetAutonomousMethodExecPolicy::Server,
	EVSNetAuthorityMethodExecPolicy::Multicast,
	EVSNetAuthorityMethodExecPolicy::Client,
	false);

int32 FVSUserQueryParams::GetUserIndex() const
{
	switch (Key)
	{
	case KeyType::UserIndex:
		return Data.UserIndex;
		
	case KeyType::UserId:
		return IPlatformInputDeviceMapper::Get().GetUserIndexForPlatformUser(Data.UserId);
		
	case KeyType::LocalPlayer:
		return Data.LocalPlayer ? IPlatformInputDeviceMapper::Get().GetUserIndexForPlatformUser(Data.LocalPlayer->GetPlatformUserId()) : INDEX_NONE;
		
	case KeyType::PlayerController:
		return Data.PlayerController ? IPlatformInputDeviceMapper::Get().GetUserIndexForPlatformUser(Data.PlayerController->GetPlatformUserId()) : INDEX_NONE;
		
	default: ;
	}

	return INDEX_NONE;
}

FPlatformUserId FVSUserQueryParams::GetUserId() const
{
	switch (Key)
	{
	case KeyType::UserIndex:
		return IPlatformInputDeviceMapper::Get().GetPlatformUserForUserIndex(Data.UserIndex);
		
	case KeyType::UserId:
		return Data.UserId;
		
	case KeyType::LocalPlayer:
		return Data.LocalPlayer ? Data.LocalPlayer->GetPlatformUserId() : FPlatformUserId();
		
	case KeyType::PlayerController:
		return Data.PlayerController ? Data.PlayerController->GetPlatformUserId() : FPlatformUserId();
		
	default: ;
	}

	return FPlatformUserId();
}

bool FVSLocationUnderCursorQueryParams::IsValid() const
{
	if (!PlayerController.IsValid()) return false;
	if (!bTraceByCollision && !bIntersectByPlane) return false;
	return true;
}