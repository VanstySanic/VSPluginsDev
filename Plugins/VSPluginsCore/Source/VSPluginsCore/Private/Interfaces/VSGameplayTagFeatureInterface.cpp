// Copyright VanstySanic. All Rights Reserved.

#include "Interfaces/VSGameplayTagFeatureInterface.h"

UVSGameplayTagFeature* IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Native() const
{
	UObject* Object = _getUObject();
	if (!Object) return nullptr;
	return Execute_GetPrimaryGameplayTagFeature(_getUObject());
}
