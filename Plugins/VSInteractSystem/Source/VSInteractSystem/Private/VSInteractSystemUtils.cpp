// Copyright VanstySanic. All Rights Reserved.

#include "VSInteractSystemUtils.h"
#include "Interact/VSInteractInterface.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/VSInteractiveInterface.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Libraries/VSActorLibrary.h"

UVSInteractSystemUtils::UVSInteractSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSInteractFeatureAgent* UVSInteractSystemUtils::GetInteractFeatureAgentFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	UVSInteractFeatureAgent* FeatureAgent = nullptr;
	if (!FeatureAgent)
	{
		if (Actor->GetClass()->ImplementsInterface(UVSInteractInterface::StaticClass()))
		{
			FeatureAgent = IVSInteractInterface::Execute_GetInteractFeatureAgent(Actor);
		}
	}
	if (!FeatureAgent)
	{
		FeatureAgent = UVSActorLibrary::FindFeatureByClassFromActor<UVSInteractFeatureAgent>(Actor);
	}
	return FeatureAgent;
}

UVSInteractiveFeatureAgent* UVSInteractSystemUtils::GetInteractiveFeatureAgentFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	UVSInteractiveFeatureAgent* FeatureAgent = nullptr;
	if (!FeatureAgent)
	{
		if (Actor->GetClass()->ImplementsInterface(UVSInteractiveInterface::StaticClass()))
		{
			FeatureAgent = IVSInteractiveInterface::Execute_GetInteractiveFeatureAgent(Actor);
		}
	}
	if (!FeatureAgent)
	{
		FeatureAgent = UVSActorLibrary::FindFeatureByClassFromActor<UVSInteractiveFeatureAgent>(Actor);
	}
	return FeatureAgent;
}
