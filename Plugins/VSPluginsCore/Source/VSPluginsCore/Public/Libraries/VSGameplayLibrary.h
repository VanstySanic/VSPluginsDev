// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSGameplayLibrary.generated.h"

struct FVSGameplayTagEventQueryContainer;
struct FGameplayTag;
struct FGameplayTagContainer;
struct FVSGameplayTagEventQuery;

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Source"))
	static bool IsInGame();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, GameplayTags, TagEvent"))
	static bool MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag());

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, GameplayTags, TagEvent"))
	static bool MatchesGameplayTagEventQueries(const FVSGameplayTagEventQueryContainer& Queries, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag());
};
