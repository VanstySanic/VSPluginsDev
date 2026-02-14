// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSGameplayLibrary.h"

UVSGameplayLibrary::UVSGameplayLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UWorld* UVSGameplayLibrary::GetPossibleGameplayWorld()
{
	if (!GEngine) return nullptr;
	
	if (UWorld* World = GEngine->GetCurrentPlayWorld(nullptr))
	{
		return World;
	}

	if (GWorld && (GWorld->IsGameWorld() || GWorld->IsPlayInEditor()))
	{
		return GWorld;
	}
	
	return nullptr;
}
