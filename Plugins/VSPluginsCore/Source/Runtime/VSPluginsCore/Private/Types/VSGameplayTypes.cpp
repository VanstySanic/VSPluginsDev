// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSGameplayTypes.h"

namespace EVSGameplayTagControllerTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event_TagsUpdated, "VSPluginsCore.GameplayTagController.Event.TagsUpdated");
}

bool FVSGameplayTagEventQuery::Matches(const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent) const
{
	if (TagEvent == FGameplayTag::EmptyTag && !bEmptyEventAsPass) return false;
	if (TagEvents.IsEmpty() && !bTagEventsEmptyAsPass) return false;
	if (!TagEvents.IsEmpty() && !(bMatchExactTagEvent ? TagEvents.HasTag(TagEvent) : TagEvents.HasTagExact(TagEvent))) return false;
	if (TagQuery.IsEmpty() && !bTagQueryEmptyAsPass) return false;
	if (!TagQuery.IsEmpty() && !TagQuery.Matches(GameplayTags)) return false;
	
	return true;
}

bool FVSGameplayTagEventQueryContainer::Matches(const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent) const
{
	if (Queries.IsEmpty() && bQueriesEmptyAsPass) return true;
	for (const FVSGameplayTagEventQuery& Query : Queries)
	{
		if (Query.Matches(GameplayTags, TagEvent))
		{
			return true;
		}
	}
	
	return false;
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

bool FVSSceneComponentQuery::Matches(const USceneComponent* Component) const
{
	if (!Component) return false;
	if (ObjectTypes.IsEmpty() && !ObjectTypesEmptyAsPass) return false;
	if (!ObjectTypes.IsEmpty() && !ObjectTypes.Contains(Component->GetCollisionObjectType())) return false;
	if (ComponentTags.IsEmpty() && !bComponentTagsEmptyAsPass) return false;
	if (!ComponentTags.IsEmpty())
	{
		bool bHasComponentTag = false;
		for (const FName& ComponentTag : ComponentTags)
		{
			if (Component->ComponentTags.Contains(ComponentTag))
			{
				bHasComponentTag = true;
				break;
			}
		}
		if (!(bHasComponentTag && !bInverseComponentTagAllowance)) return false;
	}
	if (ActorTags.IsEmpty() && !bActorTagsEmptyAsPass) return false;
	if (!ActorTags.IsEmpty())
	{
		bool bHasActorTag = false;
		for (const FName& ActorTag : ActorTags)
		{
			if (Component->GetOwner()->Tags.Contains(ActorTag))
			{
				bHasActorTag = true;
				break;
			}
		}
		if (!(bHasActorTag && !bInverseActorTagAllowance)) return false;
	}

	return true;
};