// Copyright VanstySanic. All Rights Reserved.

#include "Interfaces/VSGameplayTagFeatureInterface.h"

UVSGameplayTagFeatureBase* IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Native() const
{
	if (UObject* Object = _getUObject())
	{
		return Execute_GetPrimaryGameplayTagFeature(Object);
	}
	return nullptr;
}
