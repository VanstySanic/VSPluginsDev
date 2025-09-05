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

	template <typename T>
	static int32 GetArrayNearestElementIndex(T Source, const TArray<T>& Array);
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

template <typename T>
int32 UVSSetContainerLibrary::GetArrayNearestElementIndex(T Source, const TArray<T>& Array)
{
	static_assert(TIsArithmetic<T>::Value, "GetArrayNearestElementIndex<T>: T must be an arithmetic type.");

	if (Array.Num() == 0)
	{
		return INDEX_NONE;
	}

	if constexpr (TIsFloatingPoint<T>::Value)
	{
		if (!FMath::IsFinite(static_cast<double>(Source)))
		{
			return INDEX_NONE;
		}
	}

	int32 BestIndex = INDEX_NONE;
	long double BestDiff = std::numeric_limits<long double>::infinity();

	for (int32 i = 0; i < Array.Num(); ++i)
	{
		const T& Elem = Array[i];

		if constexpr (TIsFloatingPoint<T>::Value)
		{
			if (!FMath::IsFinite(static_cast<double>(Elem)))
			{
				continue;
			}
		}

		long double Diff = static_cast<long double>(Elem) - static_cast<long double>(Source);
		if (Diff < 0)
		{
			Diff = -Diff;
		}

		if (Diff < BestDiff)
		{
			BestDiff = Diff;
			BestIndex = i;

			if (BestDiff == static_cast<long double>(0))
			{
				break;
			}
		}
	}

	return BestIndex;
}

