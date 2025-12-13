// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSGameplayTypes.h"

namespace EVSGameplayTagControllerTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event_TagsUpdated, "VSPluginsCore.GameplayTagController.Event.TagsUpdated");
}

uint32 FVSUserQueryParams::GetUserIndex() const
{
	switch (Key)
	{
	case KeyType::UserIndex:
		return Data.UserIndex;
		
	case KeyType::UserId:
		return IPlatformInputDeviceMapper::Get().GetUserIndexForPlatformUser(Data.UserId);
		
	case KeyType::LocalPlayer:
		return Data.LocalPlayer ? IPlatformInputDeviceMapper::Get().GetUserIndexForPlatformUser(Data.LocalPlayer->GetPlatformUserId()) : 0;
		
	case KeyType::PlayerController:
		return Data.PlayerController ? IPlatformInputDeviceMapper::Get().GetUserIndexForPlatformUser(Data.PlayerController->GetPlatformUserId()) : 0;
		
	default: ;
	}

	return 0;
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

bool FVSLocationUnderCursorQueryParams::IsValid() const
{
	if (!PlayerController.IsValid()) return false;
	if (!bTraceByCollision && !bIntersectByPlane) return false;
	return true;
}