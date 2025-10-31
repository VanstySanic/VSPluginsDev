// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/VSGameplayTypes.h"
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

	/** Check if current execution is in game world (not only editor). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static bool IsInGame();

	/** Get server world time in seconds from GameState. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (WorldContext = "WorldContext"))
	static float GetServerTimeSeconds(const UObject* WorldContext);

	/** Check if a gameplay tag event query matches given tag and container. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "Query, TagEvent, GameplayTags"))
	static bool MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTag& TagEvent = FGameplayTag(), const FGameplayTagContainer& GameplayTags = FGameplayTagContainer());

	/** Check if a scene component query matches given component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Query"))
	static bool MatchesSceneComponentQuery(const FVSSceneComponentQuery& Query, const USceneComponent* SceneComponent);
	
	/** Get current screen viewport size from a player controller. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static bool GetScreenViewportSize(APlayerController* PlayerController, FVector2D& OutPosition);

	/**
	 * Get world location under cursor.
	 * Uses collision trace or plane intersection depending on query params.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (AutoCreateRefTerm = "PlaneNormal, PlanePoint"))
	static bool GetLocationUnderCursor(FVector& OutLocation, const FVSLocationUnderCursorQueryParams QueryParams = FVSLocationUnderCursorQueryParams());

	/**
	 * Suggest initial velocity for a projectile.
	 * Calculates velocity required to reach target in given time under gravity.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Projectile", meta = (AutoCreateRefTerm = "StartLocation, EndLocation, GravityDirection"))
	static FVector SuggestVelocityForProjectileMovementByTime(const FVector& StartLocation, const FVector& EndLocation, const float MovementTime = 1.f, const FVector& GravityDirection = FVector(0.0, 0.0, -1.0), float GravitySize = 980.f);

	/** Get the post process volume of the highest priority that contains the location point. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Post Process", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Location"))
	static APostProcessVolume* GetPostProcessVolumeAtLocation(UObject* WorldContext, const FVector& Location);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static FText GetRichStyledText(const FVSRichStyledText& RichStyledText);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static FString GetRichStyledTextString(const FVSRichStyledText& RichStyledText);

public:
	/**
	 * @return Whether the sweep has been blocked.
	 */
	static bool SweepSingleByShapeAndChannels(const UObject* WorldContext, FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape& Shape, const FCollisionResponseContainer& Channels, const FCollisionQueryParams& QueryParams = FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam);
};