// Copyright VanstySanic. All Rights Reserved.


#include "Classes/Queries/VSSceneComponentQueryExpression.h"

UVSSceneComponentQueryExpression::UVSSceneComponentQueryExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSSceneComponentQueryExpression::Matches(const USceneComponent* Component) const
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
			for (const TObjectPtr<UVSSceneComponentQueryExpression> Expression : Expressions)
			{
				if (Expression->Matches(Component))
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
