// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSSetContainerLibrary.generated.h"

class UKismetArrayLibrary;

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSSetContainerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	template <typename T>
	static TArray<T> GetArrayIntersection(const TArray<T>& ArrayA, const TArray<T>& ArrayB);

	template <typename T>
	static TArray<T> GetArrayUnion(const TArray<T>& ArrayA, const TArray<T>& ArrayB);
	
	template <typename T>
	static TArray<T> GetArrayComplement(const TArray<T>& ArrayA, const TArray<T>& ArrayB);

	template <typename T>
	static TArray<T> GetArrayDifference(const TArray<T>& ArrayA, const TArray<T>& ArrayB);
	
	template <typename T>
	static void RemoveArrayDuplicates(TArray<T>& Array);
};

template <typename T>
TArray<T> UVSSetContainerLibrary::GetArrayIntersection(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
{
	TSet<T> SetB(ArrayB);
	TSet<T> ResultSet;

	for (const T& Elem : ArrayA)
	{
		if (SetB.Contains(Elem))
		{
			ResultSet.Add(Elem);
		}
	}

	return ResultSet.Array();
}

template <typename T>
TArray<T> UVSSetContainerLibrary::GetArrayUnion(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
{
	TSet<T> ResultSet;

	for (const T& Elem : ArrayA)
	{
		ResultSet.Add(Elem);
	}
	for (const T& Elem : ArrayB)
	{
		ResultSet.Add(Elem);
	}

	return ResultSet.Array();
}

template <typename T>
TArray<T> UVSSetContainerLibrary::GetArrayComplement(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
{
	TSet<T> SetB(ArrayB);
	TArray<T> Result;

	for (const T& Elem : ArrayA)
	{
		if (!SetB.Contains(Elem))
		{
			Result.Add(Elem);
		}
	}

	return Result;
}

template <typename T>
TArray<T> UVSSetContainerLibrary::GetArrayDifference(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
{
	TSet<T> SetB(ArrayB);
	TArray<T> Result;

	for (const T& Elem : ArrayA)
	{
		if (!SetB.Contains(Elem))
		{
			Result.Add(Elem);
		}
	}

	return Result;
}

template <typename T>
void UVSSetContainerLibrary::RemoveArrayDuplicates(TArray<T>& Array)
{
	TSet<T> Seen;
	int32 WriteIndex = 0;

	for (int32 ReadIndex = 0; ReadIndex < Array.Num(); ++ReadIndex)
	{
		const T& Elem = Array[ReadIndex];
		if (!Seen.Contains(Elem))
		{
			Seen.Add(Elem);
			Array[WriteIndex++] = Elem;
		}
	}
	Array.SetNum(WriteIndex);
}

