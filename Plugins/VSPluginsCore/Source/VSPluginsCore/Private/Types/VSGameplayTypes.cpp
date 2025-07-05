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
