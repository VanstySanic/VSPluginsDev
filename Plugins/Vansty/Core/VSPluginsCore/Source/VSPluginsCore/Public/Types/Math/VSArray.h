// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Utility functions for common TArray set-like operations. */
struct FVSArray
{
	/** Returns elements common to both arrays (unique). */
	template <typename T>
	static TArray<T> GetArrayIntersection(const TArray<T>& ArrayA, const TArray<T>& ArrayB);

	/** Returns the union of both arrays (unique). */
	template <typename T>
	static TArray<T> GetArrayUnion(const TArray<T>& ArrayA, const TArray<T>& ArrayB);

	/** Returns elements in ArrayA that are not present in ArrayB. */
	template <typename T>
	static TArray<T> GetArrayComplement(const TArray<T>& ArrayA, const TArray<T>& ArrayB);

	/** Alias for GetArrayComplement. */
	template <typename T>
	static TArray<T> GetArrayDifference(const TArray<T>& ArrayA, const TArray<T>& ArrayB);

	/** Removes duplicate elements while preserving first-occurrence order. */
	template <typename T>
	static void RemoveArrayDuplicates(TArray<T>& Array);

	/** Returns the index of the element closest to Source, or INDEX_NONE if invalid. */
	template <typename T>
	static int32 GetArrayNearestElementIndex(const T& Source, const TArray<T>& Array);
};

template <typename T>
TArray<T> FVSArray::GetArrayIntersection(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
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
TArray<T> FVSArray::GetArrayUnion(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
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
TArray<T> FVSArray::GetArrayComplement(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
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
TArray<T> FVSArray::GetArrayDifference(const TArray<T>& ArrayA, const TArray<T>& ArrayB)
{
	return GetArrayComplement(ArrayA, ArrayB);
}

template <typename T>
void FVSArray::RemoveArrayDuplicates(TArray<T>& Array)
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
int32 FVSArray::GetArrayNearestElementIndex(const T& Source, const TArray<T>& Array)
{
	static_assert(TIsArithmetic<T>::Value, "T must be an arithmetic type.");

	if (Array.Num() == 0)
	{
		return INDEX_NONE;
	}

	/** Reject non-finite source values for floating-point types. */
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

			/** Early out on exact match. */
			if (BestDiff == static_cast<long double>(0))
			{
				break;
			}
		}
	}

	return BestIndex;
}
