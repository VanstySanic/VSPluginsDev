// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSGameplayLibrary.h"

#include "Types/VSGameplayTypes.h"

UVSGameplayLibrary::UVSGameplayLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSGameplayLibrary::IsInGame()
{
	return !GIsEditor || GIsPlayInEditorWorld || (GWorld && GWorld->HasBegunPlay());
}

bool UVSGameplayLibrary::MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent)
{
	return Query.Matches(GameplayTags, TagEvent);
}

bool UVSGameplayLibrary::MatchesGameplayTagEventQueries(const FVSGameplayTagEventQueryContainer& Queries, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent)
{
	return Queries.Matches(GameplayTags, TagEvent);
}
