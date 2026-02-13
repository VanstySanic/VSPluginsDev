// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSGameQueryTypes.h"

FVSGameplayTagEventQuery FVSGameplayTagEventQuery::Empty = FVSGameplayTagEventQuery();

bool FVSGameplayTagEventQueryParams::Matches(const FGameplayTagContainer& InTagEvents, const FGameplayTagContainer& InGameplayTags) const
{
	if (TagEvents.IsEmpty() && !bTriggerEventsEmptyAsPass) return false;
	if (InTagEvents.IsEmpty() && !bPassedInEventsEmptyAsPass) return false;
	if (!TagEvents.IsEmpty() && !InTagEvents.IsEmpty())
	{
		switch (EventMatchType)
		{
		case EGameplayContainerMatchType::Any:
			{
				if (bMatchExactTagEvents ? !InTagEvents.HasAnyExact(TagEvents) : !InTagEvents.HasAny(TagEvents))
				{
					return false;
				}
			}
			break;
			
		case EGameplayContainerMatchType::All:
			{
				if (bMatchExactTagEvents ? !InTagEvents.HasAllExact(TagEvents) : !InTagEvents.HasAll(TagEvents))
				{
					return false;
				}
			}
			break;
		}
	}
	if (TagQuery.IsEmpty() && !bTagQueryEmptyAsPass) return false;
	if (!TagQuery.IsEmpty() && !TagQuery.Matches(InGameplayTags)) return false;
	
	return true;
}

bool FVSGameplayTagEventQueryExpression::Matches(const FGameplayTagContainer& TagEvents, const FGameplayTagContainer& GameplayTags) const
{
	switch (Type) {
	case EVSQueryMatchType::Params:
		{
			int32 MatchNum = 0;
			for (const FVSGameplayTagEventQueryParams& Param : Params)
			{
				if (Param.Matches(TagEvents, GameplayTags))
				{
					MatchNum++;
				}
			}

			switch (Range) {
			case EVSQueryMatchRange::None:
				return MatchNum == 0;

			case EVSQueryMatchRange::Any:
				return MatchNum > 0;
				
			case EVSQueryMatchRange::All:
				return MatchNum > 0 && MatchNum == Params.Num();

			default: ;
			}
		}
		break;
		
	case EVSQueryMatchType::Expression:
		{
			int32 MatchNum = 0;
			for (const TInstancedStruct<FVSGameplayTagEventQueryExpression>& Expression : Expressions)
			{
				if (Expression.Get<FVSGameplayTagEventQueryExpression>().Matches(TagEvents, GameplayTags))
				{
					MatchNum++;
				}
			}

			switch (Range) {
			case EVSQueryMatchRange::None:
				return MatchNum == 0;

			case EVSQueryMatchRange::Any:
				return MatchNum > 0;
				
			case EVSQueryMatchRange::All:
				return MatchNum > 0 && MatchNum == Expressions.Num();

			default: ;
			}
		}
		break;
		
	default: ;
	}

	return false;
}

FVSGameplayTagEventQuery FVSGameplayTagEventQuery::GetEmptyPass()
{
	FVSGameplayTagEventQuery Query;
	Query.RootExpression.Range = EVSQueryMatchRange::None;
	Query.RootExpression.Type = EVSQueryMatchType::Params;
	return Query;
}

bool FVSGameplayTagEventQuery::Matches(const FGameplayTagContainer& TagEvents, const FGameplayTagContainer& GameplayTags) const
{
	return RootExpression.Matches(TagEvents, GameplayTags);
}


bool FVSSceneComponentQueryParams::Matches(const USceneComponent* Component) const
{
	if (!Component) return false;
	
	if (ComponentClasses.IsEmpty() && !bComponentClassesEmptyAsPass) return false;
	if (!ComponentClasses.IsEmpty())
	{
		const bool bHasComponentClass = ComponentClasses.ContainsByPredicate(
			[Component] (const TSubclassOf<USceneComponent>& Class)
			{
				return Class && Component->IsA(Class);
			});

		if (bInverseClassAllowance ? bHasComponentClass : !bHasComponentClass) return false;
	}
	
	if (ObjectTypes.IsEmpty() && !bObjectTypesEmptyAsPass) return false;
	if (!ObjectTypes.IsEmpty())
	{
		const bool bHasObjectType = ObjectTypes.Contains(Component->GetCollisionObjectType());
		if (bInverseObjectTypes ? bHasObjectType : !bHasObjectType) return false; 
	}
	
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
		if (bInverseComponentTagAllowance ? bHasComponentTag : !bHasComponentTag) return false;
	}
	
	if (ActorTags.IsEmpty() && !bActorTagsEmptyAsPass) return false;
	if (!ActorTags.IsEmpty())
	{
		if (!Component->GetOwner()) return false;
		
		bool bHasActorTag = false;
		for (const FName& ActorTag : ActorTags)
		{
			if (Component->GetOwner()->Tags.Contains(ActorTag))
			{
				bHasActorTag = true;
				break;
			}
		}
		
		if (bInverseActorTagAllowance ? bHasActorTag : !bHasActorTag) return false;
	}

	return true;
}

bool FVSSceneComponentQueryExpression::Matches(const USceneComponent* Component) const
{
	switch (Type) {
	case EVSQueryMatchType::Params:
		{
			int32 MatchNum = 0;
			for (const FVSSceneComponentQueryParams& Param : Params)
			{
				if (Param.Matches(Component))
				{
					MatchNum++;
				}
			}

			switch (Range) {
			case EVSQueryMatchRange::None:
				return MatchNum == 0;

			case EVSQueryMatchRange::Any:
				return MatchNum > 0;
				
			case EVSQueryMatchRange::All:
				return MatchNum > 0 && MatchNum == Params.Num();

			default: ;
			}
		}
		break;
		
	case EVSQueryMatchType::Expression:
		{
			int32 MatchNum = 0;
			for (const TInstancedStruct<FVSSceneComponentQueryExpression>& Expression : Expressions)
			{
				if (Expression.Get<FVSSceneComponentQueryExpression>().Matches(Component))
				{
					MatchNum++;
				}
			}

			switch (Range) {
			case EVSQueryMatchRange::None:
				return MatchNum == 0;

			case EVSQueryMatchRange::Any:
				return MatchNum > 0;
				
			case EVSQueryMatchRange::All:
				return MatchNum > 0 && MatchNum == Expressions.Num();

			default: ;
			}
		}
		break;
		
	default: ;
	}

	return false;
}

bool FVSSceneComponentQuery::Matches(const USceneComponent* Component) const
{
	return RootExpression.Matches(Component);
}

