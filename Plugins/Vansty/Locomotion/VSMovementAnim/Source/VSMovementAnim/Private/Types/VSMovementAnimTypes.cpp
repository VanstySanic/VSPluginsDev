// Copyright VanstySanic. All Rights Reserved.


#include "Types/VSMovementAnimTypes.h"

UAnimSequence* FVSKeyedAnimSequenceMapContainer::GetAnimSequence(const FVSGameplayTagKey& InKeySet) const
{
	for (const auto& Map : Maps)
	{
		if (Map.Map.Contains(InKeySet))
		{
			return Map.Map.FindRef(InKeySet);
		}
	}
	return nullptr;
}
