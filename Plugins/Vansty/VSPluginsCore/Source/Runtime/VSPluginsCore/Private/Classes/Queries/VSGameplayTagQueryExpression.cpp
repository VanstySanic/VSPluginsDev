// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Queries/VSGameplayTagQueryExpression.h"

UVSGameplayTagQueryExpression::UVSGameplayTagQueryExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool UVSGameplayTagQueryExpression::Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const
{
	switch (Type) {
	case EVSQueryMatchType::Params:
		{
			int32 MatchNum = 0;
			for (const FVSGameplayTagEventQueryParams& Param : Params)
			{
				if (Param.Matches(GameplayTags, TagEvent))
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
			for (const TObjectPtr<UVSGameplayTagQueryExpression> Node : Expressions)
			{
				if (Node && Node->Matches(TagEvent, GameplayTags))
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
