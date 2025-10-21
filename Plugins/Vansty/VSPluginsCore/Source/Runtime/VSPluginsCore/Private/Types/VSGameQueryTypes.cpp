// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSQueryMatchTypes.h"
#include "Classes/Queries/VSGameplayTagQueryExpression.h"
#include "Classes/Queries/VSSceneComponentQueryExpression.h"

bool FVSGameplayTagEventQueryParams::Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const
{
	if (TagEvent == FGameplayTag::EmptyTag && !bEmptyEventAsPass) return false;
	if (TagEvents.IsEmpty() && !bTagEventsEmptyAsPass) return false;
	if (!TagEvents.IsEmpty() && !(bMatchExactTagEvent ? TagEvents.HasTag(TagEvent) : TagEvents.HasTagExact(TagEvent))) return false;
	if (TagQuery.IsEmpty() && !bTagQueryEmptyAsPass) return false;
	if (!TagQuery.IsEmpty() && !TagQuery.Matches(GameplayTags)) return false;
	
	return true;
}

bool FVSGameplayTagEventQuery::Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const
{
	return RootExpression && RootExpression->Matches(TagEvent, GameplayTags);
}


bool FVSSceneComponentQueryParams::Matches(const USceneComponent* Component) const
{
	if (!Component) return false;
	if (ComponentClasses.IsEmpty() && !bComponentClassesEmptyAsPass) return false;
	if (!ComponentClasses.IsEmpty() && !(ComponentClasses.Contains(Component->GetClass()) && !bInverseClassAllowance)) return false;
	if (ObjectTypes.IsEmpty() && !bObjectTypesEmptyAsPass) return false;
	if (!ObjectTypes.IsEmpty() && !(ObjectTypes.Contains(Component->GetCollisionObjectType()) && !bInverseObjectTypes)) return false;
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
}

bool FVSSceneComponentQuery::Matches(const USceneComponent* Component) const
{
	return RootExpression && RootExpression->Matches(Component);
}

