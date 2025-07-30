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
	
	UFUNCTION(BlueprintCallable, Category = "MetaData")
	static FName GetClassPropertyMetaData(UClass* Class, FName PropertyName, FName MetaDataName);

	UFUNCTION(BlueprintCallable, Category = "MetaData")
	static FName GetClassMapKeyPropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName);

	UFUNCTION(BlueprintCallable, Category = "MetaData")
	static FName GetClassMapValuePropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName);
	
	UFUNCTION(BlueprintCallable, Category = "MetaData")
	static bool SetClassPropertyMetaData(UClass* Class, FName PropertyName, FName MetaDataName, FName MetaData);

	UFUNCTION(BlueprintCallable, Category = "MetaData")
	static bool SetClassMapKeyPropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName, FName MetaData);

	UFUNCTION(BlueprintCallable, Category = "MetaData")
	static bool SetClassMapValuePropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName, FName MetaData);
};
