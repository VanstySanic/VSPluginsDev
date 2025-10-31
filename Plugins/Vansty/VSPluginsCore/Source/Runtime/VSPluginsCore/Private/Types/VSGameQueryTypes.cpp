// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSQueryMatchTypes.h"

FVSGameplayTagEventQuery FVSGameplayTagEventQuery::Empty = FVSGameplayTagEventQuery();

bool FVSGameplayTagEventQueryParams::Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const
{
	if (TagEvent == FGameplayTag::EmptyTag && !bEmptyEventAsPass) return false;
	if (TagEvents.IsEmpty() && !bTagEventsEmptyAsPass) return false;
	if (!TagEvents.IsEmpty() && !(bMatchExactTagEvent ? TagEvents.HasTag(TagEvent) : TagEvents.HasTagExact(TagEvent))) return false;
	if (TagQuery.IsEmpty() && !bTagQueryEmptyAsPass) return false;
	if (!TagQuery.IsEmpty() && !TagQuery.Matches(GameplayTags)) return false;
	
	return true;
}

bool FVSGameplayTagQueryExpression::Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const
{
	switch (Type) {
	case EVSQueryMatchType::Params:
		{
			int32 MatchNum = 0;
			for (const FVSGameplayTagEventQueryParams& Param : Params)
			{
				if (Param.Matches(TagEvent, GameplayTags))
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
				return MatchNum == Params.Num();

			default: ;
			}
		}
		break;
		
	case EVSQueryMatchType::Expression:
		{
			int32 MatchNum = 0;
			for (const TInstancedStruct<FVSGameplayTagQueryExpression>& Expression : Expressions)
			{
				if (Expression.Get<FVSGameplayTagQueryExpression>().Matches(TagEvent, GameplayTags))
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
				return MatchNum == Params.Num();

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

bool FVSGameplayTagEventQuery::Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const
{
	return RootExpression.Matches(TagEvent, GameplayTags);
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
				return MatchNum == Expressions.Num();

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
				return MatchNum == Expressions.Num();

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

