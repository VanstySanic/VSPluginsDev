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
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Source"))
	static float GetServerTimeSeconds(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, GameplayTags, TagEvent"))
	static bool MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag());

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, GameplayTags, TagEvent"))
	static bool MatchesGameplayTagEventQueries(const FVSGameplayTagEventQueryContainer& Queries, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag());

	/**
	 * @return Whether the sweep has been blocked.
	 */
	static bool SweepSingleByShapeAndChannels(const UObject* WorldContext, FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape& Shape, const FCollisionResponseContainer& Channels, const FCollisionQueryParams& QueryParams = FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Projectile")
	static FVector SuggestVelocityForProjectileMovementByTime(const FVector& StartLocation, const FVector& EndLocation, const float MovementTime = 1.f, const FVector& GravityDirection = FVector(0.0, 0.0, -1.0), float GravitySize = 980.f);
};