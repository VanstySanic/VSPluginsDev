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

	/** Determines whether this object is a template object by checking flags on the object and the outer chain. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object"))
	static bool IsObjectTemplate(UObject* Object);

	/** Thread-safe IsValid wrapper. */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (BlueprintThreadSafe, DefaultToSelf = "Source"))
	static bool IsValidThreadSafe(const UObject* Object);

	/** Check if an object is inside the given outer. */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Source"))
	static bool HasOuterObject(const UObject* Object, const UObject* Outer);

	/** Get typed outer object of given class. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UObject* GetTypedOuterObject(const UObject* Object, const TSubclassOf<UObject> Class);

	/** Get outer object that implements given interface. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object"))
	static UObject* GetImplementingOuterObject(const UObject* Object, const TSubclassOf<UInterface> Interface);

	/** Add tick prerequisite relationship between two objects. */
	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Source"))
	static void AddTickPrerequisiteObject(UObject* Source, UObject* PrerequisiteObject);

	/** Remove tick prerequisite relationship between two objects. */
	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Source"))
	static void RemoveTickPrerequisiteObject(UObject* Source, UObject* PrerequisiteObject);
	
	/**
	 * Find feature on object by class.
	 * Searches both actor components and object properties.
	 */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UVSObjectFeature* GetFeatureByClassFromObject(UObject* Object, TSubclassOf<UVSObjectFeature> Class);

	/**
	 * Find feature on object by feature name.
	 * Searches both actor components and object properties.
	 */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object"))
	static UVSObjectFeature* GetFeatureByNameFromObject(UObject* Object, FName Name);

public:
	/**
	 * Find feature on object by class.
	 * Searches both actor components and object properties.
	 */
	template <typename T>
	static T* FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class = T::StaticClass());

	/** Get the possible tick function pointer from an object. */
	static FTickFunction* GetTickFunctionPtrFromObject(UObject* Object);
};

template <typename T>
T* UVSObjectLibrary::FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class)
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetFeatureByClassFromObject(Object, Class));
}