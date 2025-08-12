// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSActorLibrary.generated.h"

class UVSGameplayTagController;
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Source"))
	static bool IsActorLocal(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Source"))
	static bool IsActorLocalRoleAuthorityOrAutonomous(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Actor")
	static AActor* DuplicateActor(AActor* Actor, const FTransform& SpawnTransform = FTransform());
	template<typename T>
	static T* DuplicateActor(T* Actor, const FTransform& SpawnTransform = FTransform::Identity);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UActorComponent* GetActorComponentByName(const AActor* Actor, FName ComponentName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UCameraComponent* GetActiveCameraFromActor(const AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UVSGameplayTagController* GetGameplayTagControllerFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Feature"))
	static UVSObjectFeature* GetFeatureByClassFromActor(AActor* Object, TSubclassOf<UVSObjectFeature> Class);
	template <typename T>
	static T* FindFeatureByClassFromActor(AActor* Actor, TSubclassOf<T> Class = T::StaticClass());
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UVSObjectFeature* GetFeatureByNameFromActor(AActor* Actor, FName Name);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UAbilitySystemComponent* GetAbilitySystemComponentFormActor(AActor* Actor);


	/**
	 * Whether the character is on walkable floor.
	 * @param ToleranceToFloor The vertical height tolerance form the character's capsule lower sphere shape.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	static bool IsCharacterOnWalkableFloor(const ACharacter* Character, const float ToleranceToFloor = 0.f, const bool bConsiderCollisionOffset = true);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	static FVector GetCharacterRootLocation(const ACharacter* Character, const float VerticalOffset = 0.f /** 2.f */, float UnscaledHalfHeightOverride = 0.f);
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

template <typename T>
T* UVSActorLibrary::FindFeatureByClassFromActor(AActor* Actor, TSubclassOf<T> Class)
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetFeatureByClassFromActor(Actor, Class));
}
