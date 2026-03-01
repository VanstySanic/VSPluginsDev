// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSContainerLibrary.generated.h"

/**
 * Blueprint-accessible container helpers with wildcard (custom thunk) support.
 */
UCLASS()
class VSPLUGINSCORE_API UVSContainerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns elements common to both arrays (unique). */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Array",
		meta = (DisplayName = "Array Intersection", CompactNodeTitle = "Intersect",
			ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayA,ArrayB,Result"))
	static void ArrayIntersection(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result);

	/** Returns the union of both arrays (unique). */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Array",
		meta = (DisplayName = "Array Union", CompactNodeTitle = "Union",
			ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayA,ArrayB,Result"))
	static void ArrayUnion(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result);

	/** Returns elements in ArrayA that are not present in ArrayB (unique). */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Array",
		meta = (DisplayName = "Array Complement", CompactNodeTitle = "Complement",
			ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayA,ArrayB,Result"))
	static void ArrayComplement(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result);

	/** Alias for Array Complement. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Array",
		meta = (DisplayName = "Array Difference", CompactNodeTitle = "Diff",
			ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayA,ArrayB,Result"))
	static void ArrayDifference(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result);

	/** Removes duplicate elements while preserving first-occurrence order. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Array",
		meta = (DisplayName = "Array Remove Duplicates", CompactNodeTitle = "Dedup",
			ArrayParm = "Array", ArrayTypeDependentParams = "Array"))
	static void ArrayRemoveDuplicates(UPARAM(ref) TArray<int32>& Array);

	
	/** Returns elements common to both sets. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Set",
		meta = (DisplayName = "Set Intersection", CompactNodeTitle = "Intersect",
			SetParam = "SetA,SetB,Result", SetTypeDependentParams = "SetA,SetB,Result"))
	static void SetIntersection(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result);

	/** Returns the union of both sets. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Set",
		meta = (DisplayName = "Set Union", CompactNodeTitle = "Union",
			SetParam = "SetA,SetB,Result", SetTypeDependentParams = "SetA,SetB,Result"))
	static void SetUnion(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result);

	/** Returns elements in SetA that are not present in SetB. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Set",
		meta = (DisplayName = "Set Complement", CompactNodeTitle = "Complement",
			SetParam = "SetA,SetB,Result", SetTypeDependentParams = "SetA,SetB,Result"))
	static void SetComplement(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result);

	/** Alias for Set Complement. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Set",
		meta = (DisplayName = "Set Difference", CompactNodeTitle = "Diff",
			SetParam = "SetA,SetB,Result", SetTypeDependentParams = "SetA,SetB,Result"))
	static void SetDifference(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result);

	
	/** Returns keys present in both maps. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Map",
		meta = (DisplayName = "Map Key Intersection", CompactNodeTitle = "KeyInter",
			MapParam = "MapA,MapB", SetParam = "ResultKeys", MapTypeDependentParams = "MapA,MapB", SetTypeDependentParams = "ResultKeys"))
	static void MapKeyIntersection(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB, TSet<int32>& ResultKeys);

	/** Returns keys present in MapA but not MapB. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Map",
		meta = (DisplayName = "Map Key Complement", CompactNodeTitle = "KeyComp",
			MapParam = "MapA,MapB", SetParam = "ResultKeys", MapTypeDependentParams = "MapA,MapB", SetTypeDependentParams = "ResultKeys"))
	static void MapKeyComplement(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB, TSet<int32>& ResultKeys);

	/** Copies MapA and overrides with MapB when keys collide. */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Container|Map",
		meta = (DisplayName = "Map Merge Override", CompactNodeTitle = "Merge",
			MapParam = "MapA,MapB,Result", MapTypeDependentParams = "MapA,MapB,Result"))
	static void MapMergeOverride(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB, TMap<int32, int32>& Result);

	/** Returns true if both maps contain the same key/value pairs. */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "Container|Map",
		meta = (DisplayName = "Map Equal", CompactNodeTitle = "Equal",
			MapParam = "MapA,MapB", MapTypeDependentParams = "MapA,MapB"))
	static bool MapEqual(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB);

private:
	/** CustomThunk exec handlers (wildcard runtime marshalling). */
	DECLARE_FUNCTION(execArrayIntersection);
	DECLARE_FUNCTION(execArrayUnion);
	DECLARE_FUNCTION(execArrayComplement);
	DECLARE_FUNCTION(execArrayDifference);
	DECLARE_FUNCTION(execArrayRemoveDuplicates);
	
	DECLARE_FUNCTION(execSetIntersection);
	DECLARE_FUNCTION(execSetUnion);
	DECLARE_FUNCTION(execSetComplement);
	DECLARE_FUNCTION(execSetDifference);
	
	DECLARE_FUNCTION(execMapKeyIntersection);
	DECLARE_FUNCTION(execMapKeyComplement);
	DECLARE_FUNCTION(execMapMergeOverride);
	DECLARE_FUNCTION(execMapEqual);
};
