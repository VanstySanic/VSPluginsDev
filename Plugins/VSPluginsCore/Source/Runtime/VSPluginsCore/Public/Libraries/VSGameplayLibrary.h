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
struct FVSSceneComponentQuery;

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static bool IsInGame();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (WorldContext = "WorldContext"))
	static float GetServerTimeSeconds(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, GameplayTags, TagEvent"))
	static bool MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag());

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, GameplayTags, TagEvent"))
	static bool MatchesGameplayTagEventQueries(const FVSGameplayTagEventQueryContainer& Queries, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag());

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Query"))
	static bool MatchesSceneComponentQuery(const USceneComponent* SceneComponent, const FVSSceneComponentQuery& Query);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Object"))
	static APawn* GetPawnFromSubObject(UObject* Object);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Object"))
	static AController* GetControllerFromSubObject(UObject* Object);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Projectile", meta = (AutoCreateRefTerm = "StartLocation, EndLocation, GravityDirection"))
	static FVector SuggestVelocityForProjectileMovementByTime(const FVector& StartLocation, const FVector& EndLocation, const float MovementTime = 1.f, const FVector& GravityDirection = FVector(0.0, 0.0, -1.0), float GravitySize = 980.f);

	/** Get the post process volume of the highest priority that contains the location point. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Post Process", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Location"))
	static APostProcessVolume* GetPostProcessVolumeAtLocation(UObject* WorldContext, const FVector& Location);
	
	/**
	 * @return Whether the sweep has been blocked.
	 */
	static bool SweepSingleByShapeAndChannels(const UObject* WorldContext, FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape& Shape, const FCollisionResponseContainer& Channels, const FCollisionQueryParams& QueryParams = FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam);
};