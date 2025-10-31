// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSMetaDataLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSMetaDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaData")
	static FName GetPropertyMetaDataFormClass(UClass* Class, FName PropertyName, FName MetaDataName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaData")
	static FName GetMapKeyPropertyMetaDataFormClass(UClass* Class, FName MapPropertyName, FName MetaDataName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaData")
	static FName GetMapValuePropertyMetaDataFromClass(UClass* Class, FName MapPropertyName, FName MetaDataName);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaData")
	static bool SetPropertyMetaDataForClass(UClass* Class, FName PropertyName, FName MetaDataName, FName MetaData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaData")
	static bool SetMapKeyPropertyMetaDataForClass(UClass* Class, FName MapPropertyName, FName MetaDataName, FName MetaData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaData")
	static bool SetMapValuePropertyMetaDataForClass(UClass* Class, FName MapPropertyName, FName MetaDataName, FName MetaData);
};
