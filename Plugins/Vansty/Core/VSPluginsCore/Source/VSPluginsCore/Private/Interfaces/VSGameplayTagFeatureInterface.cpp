// Copyright VanstySanic. All Rights Reserved.

#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSObjectLibrary.h"

UVSGameplayTagFeatureBase* IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Native() const
{
	if (UObject* Object = _getUObject())
	{
		return Execute_GetPrimaryGameplayTagFeature(Object);
	}
	return nullptr;
}

UVSGameplayTagFeatureBase* IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation() const
{
	UObject* Object = _getUObject();
	if (!Object) return nullptr;

	AActor* Actor = Cast<AActor>(Object);
	if (!Actor) Actor = Object->GetTypedOuter<AActor>();
	if (!Actor) return nullptr;
	
	if (Actor->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
	{
		if (UVSGameplayTagFeatureBase* GameplayTagFeatureBase = Execute_GetPrimaryGameplayTagFeature(Actor))
		{
			return GameplayTagFeatureBase;	
		}
	}
	
	return UVSObjectLibrary::FindFeatureByClassFromObject<UVSGameplayTagFeatureBase>(Actor);
}
