// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSActorLibrary.generated.h"

class UCameraComponent;
class UVSObjectFeature;

/**
 * Blueprint actor utility helpers.
 */
UCLASS()
class VSPLUGINSCORE_API UVSActorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Returns true if this actor is considered locally controlled in net play. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static bool IsActorNetLocal(const AActor* Actor);
	
	/** Returns true if local role is authority or autonomous. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static bool IsActorNetLocalRoleAuthorityOrAutonomous(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Actor")
	static AActor* DuplicateActor(AActor* Actor, const FTransform& SpawnTransform = FTransform());
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UActorComponent* GetActorComponentByName(const AActor* Actor, FName ComponentName);
	
	/** Resolve the pawn from an actor or its owner chain. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static APawn* GetPawnFromActor(AActor* Actor);

	/** Resolve the player state from an actor or its owner chain. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static APlayerState* GetPlayerStateFromActor(AActor* Actor);
	
	/** Resolve the controller from an actor or its owner chain. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static AController* GetControllerFromActor(AActor* Actor);
	
	/** Returns the active camera component, traversing view targets when needed. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UCameraComponent* GetActiveCameraFromActor(const AActor* Actor);

public:
	template<typename T>
	static T* DuplicateActor(T* Actor, const FTransform& SpawnTransform = FTransform::Identity);
};

template <typename T>
T* UVSActorLibrary::DuplicateActor(T* Actor, const FTransform& SpawnTransform)
{
	if (!Actor) return nullptr;

	UWorld* World = Actor->GetWorld();
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.Template = Actor;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Name = MakeUniqueObjectName(World, T::StaticClass(), Actor->GetFName());
	
	T* NewActor = World->SpawnActor<T>(Actor->GetClass(), SpawnTransform, Params);
	return NewActor;
}
