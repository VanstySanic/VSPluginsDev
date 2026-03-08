// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSTreeQueryTypes.h"
#include "Engine/EngineTypes.h"

FVSGameplayTagEventQuery FVSGameplayTagEventQuery::Empty = FVSGameplayTagEventQuery();

namespace VSTreeQueryTypesPrivate
{
	static bool EvaluateRange(const int32 MatchNum, const int32 CandidateNum, const EVSElementRange::Type Range)
	{
		switch (Range)
		{
		case EVSElementRange::None:
			return MatchNum == 0;
		case EVSElementRange::Any:
			return MatchNum > 0;
		case EVSElementRange::All:
			return CandidateNum > 0 && MatchNum == CandidateNum;
		default:
			return false;
		}
	}
}

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
	case EVSTreeQueryMatchType::Param:
		{
			int32 MatchNum = 0;
			for (const FVSGameplayTagEventQueryParams& Param : Params)
			{
				if (Param.Matches(TagEvents, GameplayTags))
				{
					MatchNum++;
				}
			}

			return VSTreeQueryTypesPrivate::EvaluateRange(MatchNum, Params.Num(), Range);
		}
		break;
		
	case EVSTreeQueryMatchType::Expression:
		{
			int32 MatchNum = 0;
			for (const TInstancedStruct<FVSGameplayTagEventQueryExpression>& Expression : Expressions)
			{
				if (!Expression.IsValid()) continue;
				if (Expression.Get<FVSGameplayTagEventQueryExpression>().Matches(TagEvents, GameplayTags))
				{
					MatchNum++;
				}
			}

			return VSTreeQueryTypesPrivate::EvaluateRange(MatchNum, Expressions.Num(), Range);
		}
		break;
		
	default: ;
	}

	return false;
}

FVSGameplayTagEventQuery FVSGameplayTagEventQuery::GetEmptyPass()
{
	FVSGameplayTagEventQuery Query;
	Query.RootExpression.Range = EVSElementRange::None;
	Query.RootExpression.Type = EVSTreeQueryMatchType::Param;
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

		if (bInverseClassAllowance ? bHasComponentClass : !bHasComponentClass)
		{
			return false;
		}
	}
	
	if (ObjectTypes.IsEmpty() && !bObjectTypesEmptyAsPass) return false;
	if (!ObjectTypes.IsEmpty())
	{
		const EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(Component->GetCollisionObjectType());
		const bool bHasObjectType = ObjectTypes.Contains(ObjectType);
		if (bInverseObjectTypes ? bHasObjectType : !bHasObjectType)
		{
			return false;
		}
	}
	
	if (ComponentTags.IsEmpty() && !bComponentTagsEmptyAsPass) return false;
	if (!ComponentTags.IsEmpty())
	{
		int32 MatchNum = 0;
		for (const FName& ComponentTag : ComponentTags)
		{
			if (Component->ComponentTags.Contains(ComponentTag))
			{
				MatchNum++;
			}
		}
		if (!VSTreeQueryTypesPrivate::EvaluateRange(MatchNum, ComponentTags.Num(), ComponentTagRange))
		{
			return false;
		}
	}
	
	if (ActorTags.IsEmpty() && !bActorTagsEmptyAsPass) return false;
	if (!ActorTags.IsEmpty())
	{
		const AActor* Owner = Component->GetOwner();
		if (!Owner) return false;

		int32 MatchNum = 0;
		for (const FName& ActorTag : ActorTags)
		{
			if (Owner->Tags.Contains(ActorTag))
			{
				MatchNum++;
			}
		}
		if (!VSTreeQueryTypesPrivate::EvaluateRange(MatchNum, ActorTags.Num(), ActorTagRange))
		{
			return false;
		}
	}

	return true;
}

bool FVSSceneComponentQueryExpression::Matches(const USceneComponent* Component) const
{
	switch (Type) {
	case EVSTreeQueryMatchType::Param:
		{
			int32 MatchNum = 0;
			for (const FVSSceneComponentQueryParams& Param : Params)
			{
				if (Param.Matches(Component))
				{
					MatchNum++;
				}
			}

			return VSTreeQueryTypesPrivate::EvaluateRange(MatchNum, Params.Num(), Range);
		}
		break;
		
	case EVSTreeQueryMatchType::Expression:
		{
			int32 MatchNum = 0;
			for (const TInstancedStruct<FVSSceneComponentQueryExpression>& Expression : Expressions)
			{
				if (Expression.IsValid())
				{
					if (Expression.Get<FVSSceneComponentQueryExpression>().Matches(Component))
					{
						MatchNum++;
					}
				}
			}

			return VSTreeQueryTypesPrivate::EvaluateRange(MatchNum, Expressions.Num(), Range);
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
