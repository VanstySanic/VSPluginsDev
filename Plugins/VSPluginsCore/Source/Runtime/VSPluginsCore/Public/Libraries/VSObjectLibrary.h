// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSObjectLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSObjectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

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

private:
	static FTickFunction* GetTickFunction(UObject* Object);
};