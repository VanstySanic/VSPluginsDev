// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSActorLibrary.generated.h"

class UAbilitySystemComponent;
class UCameraComponent;
class UVSObjectFeature;

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSActorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Source"))
	static bool IsActorNetLocal(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Source"))
	static bool IsActorNetLocalRoleAuthorityOrAutonomous(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Actor")
	static AActor* DuplicateActor(AActor* Actor, const FTransform& SpawnTransform = FTransform());
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UActorComponent* GetActorComponentByName(const AActor* Actor, FName ComponentName);
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Object"))
	static APawn* GetPawnFromActor(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Object"))
	static APlayerState* GetPlayerStateFromActor(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Object"))
	static AController* GetControllerFromActor(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UCameraComponent* GetActiveCameraFromActor(const AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UAbilitySystemComponent* GetAbilitySystemComponentFormActor(AActor* Actor);

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