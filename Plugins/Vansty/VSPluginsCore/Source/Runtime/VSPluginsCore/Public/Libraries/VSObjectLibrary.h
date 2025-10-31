// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSObjectLibrary.generated.h"

class UVSObjectFeature;

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSObjectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Check if object is world-related (actor, component, subsystem, etc.). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object")
	static bool IsObjectWorldRelated(const UObject* InObject);

	/** Thread-safe IsValid wrapper. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (BlueprintThreadSafe, DefaultToSelf = "Source"))
	static bool IsValidThreadSafe(const UObject* Object);

	/** Check if an object is inside the given outer. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Source"))
	static bool IsObjectInOuter(const UObject* Source, const UObject* PossibleOuterObject);

	/** Get typed outer object of given class. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UObject* GetTypedOuterObject(const UObject* Object, const TSubclassOf<UObject> Class);

	/** Get outer object that implements given interface. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object"))
	static UObject* GetImplementingOuterObject(const UObject* Object, const TSubclassOf<UInterface> Interface);

	/** Add tick prerequisite relationship between two objects. */
	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Target"))
	static void AddTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject);

	/** Remove tick prerequisite relationship between two objects. */
	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Target"))
	static void RemoveTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject);

	/**
	 * Find feature on object by class.
	 * Searches both actor components and object properties.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UVSObjectFeature* GetFeatureByClassFromObject(UObject* Object, TSubclassOf<UVSObjectFeature> Class);

	/**
	 * Find feature on object by feature name.
	 * Searches both actor components and object properties.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Actor"))
	static UVSObjectFeature* GetFeatureByNameFromObject(UObject* Object, FName Name);

public:
	template <typename T>
	static T* FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class = T::StaticClass());
	
private:
	static FTickFunction* GetTickFunctionFromObject(UObject* Object);
};



template <typename T>
T* UVSObjectLibrary::FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class)
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetFeatureByClassFromObject(Object, Class));
}