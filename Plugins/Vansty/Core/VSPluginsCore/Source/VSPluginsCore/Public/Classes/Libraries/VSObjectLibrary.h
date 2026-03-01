// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSObjectLibrary.generated.h"

class UVSObjectFeature;

/**
 * Blueprint object utility helpers.
 */
UCLASS()
class VSPLUGINSCORE_API UVSObjectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Determines whether this object is a template object by checking flags on the object and the outer chain. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object"))
	static bool IsObjectTemplate(UObject* Object);

	/** Thread-safe UObject validity check for Blueprint utility usage. */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (BlueprintThreadSafe, DefaultToSelf = "Source"))
	static bool IsValidThreadSafe(const UObject* Object);

	/** Check if an object is inside the given outer. */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Source"))
	static bool HasOuterObject(const UObject* Object, const UObject* Outer);

	/** Returns first outer that is assignable to Class. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UObject* GetTypedOuterObject(const UObject* Object, const TSubclassOf<UObject> Class);

	/** Returns first outer that implements Interface. */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Object", meta = (DefaultToSelf = "Object"))
	static UObject* GetImplementingOuterObject(const UObject* Object, const TSubclassOf<UInterface> Interface);

	/** Adds a tick prerequisite edge: Source ticks after PrerequisiteObject. */
	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Source"))
	static void AddTickPrerequisiteObject(UObject* Source, UObject* PrerequisiteObject);

	/** Removes a previously added tick prerequisite edge. */
	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Source"))
	static void RemoveTickPrerequisiteObject(UObject* Source, UObject* PrerequisiteObject);
	
	/** Returns first feature matching Class from actor components or object properties. */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UVSObjectFeature* GetFeatureByClassFromObject(UObject* Object, TSubclassOf<UVSObjectFeature> Class);

	/** Returns first feature matching Name from actor components or object properties. */
	UFUNCTION(BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object"))
	static UVSObjectFeature* GetFeatureByNameFromObject(UObject* Object, FName Name);

public:
	/** Exports a property value to text using Unreal property serialization rules. */
	static FString ExportPropertyToText(UObject* Object, FName PropertyName);
	/** Imports a property value from text using Unreal property serialization rules. */
	static bool ImportPropertyFromText(UObject* Object, FName PropertyName, const FString& Text);

	/** Typed wrapper of GetFeatureByClassFromObject. */
	template <typename T>
	static T* FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class = T::StaticClass());

	/** Resolves tick function pointer from an object if it exposes one. */
	static FTickFunction* GetTickFunctionPtrFromObject(UObject* Object);};

template <typename T>
T* UVSObjectLibrary::FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class)
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetFeatureByClassFromObject(Object, Class));
}
