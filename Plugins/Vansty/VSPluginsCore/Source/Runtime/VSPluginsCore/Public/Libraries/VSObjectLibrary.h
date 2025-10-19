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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static bool IsObjectWorldRelated(const UObject* InObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (BlueprintThreadSafe, DefaultToSelf = "Source"))
	static bool IsValidThreadSafe(const UObject* Object);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Source"))
	static bool IsObjectInOuter(const UObject* Source, const UObject* PossibleOuterObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UObject* GetTypedOuterObject(const UObject* Object, const TSubclassOf<UObject> Class);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object", meta = (DefaultToSelf = "Object"))
	static UObject* GetImplementingOuterObject(const UObject* Object, const TSubclassOf<UInterface> Interface);


	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Target"))
	static void AddTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject);

	UFUNCTION(BlueprintCallable, Category = "Object", meta = (DefaultToSelf = "Target"))
	static void RemoveTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Object", DeterminesOutputType = "Class"))
	static UVSObjectFeature* GetFeatureByClassFromObject(UObject* Object, TSubclassOf<UVSObjectFeature> Class);
	template <typename T>
	static T* FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class = T::StaticClass());
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UVSObjectFeature* GetFeatureByNameFromObject(UObject* Object, FName Name);
	
private:
	static FTickFunction* GetTickFunctionFromObject(UObject* Object);
};



template <typename T>
T* UVSObjectLibrary::FindFeatureByClassFromObject(UObject* Object, TSubclassOf<T> Class)
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetFeatureByClassFromObject(Object, Class));
}