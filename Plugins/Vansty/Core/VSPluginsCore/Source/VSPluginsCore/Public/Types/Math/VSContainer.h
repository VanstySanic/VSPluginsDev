// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Utility functions for common TArray set-like operations.
 */
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

/**
 * Utility functions for common TSet operations.
 */
struct FVSSet
{
	/** Returns elements common to both sets. */
	template <typename T>
	static TSet<T> GetSetIntersection(const TSet<T>& SetA, const TSet<T>& SetB);

	/** Returns the union of both sets. */
	template <typename T>
	static TSet<T> GetSetUnion(const TSet<T>& SetA, const TSet<T>& SetB);

	/** Returns elements in SetA that are not present in SetB. */
	template <typename T>
	static TSet<T> GetSetComplement(const TSet<T>& SetA, const TSet<T>& SetB);

	/** Alias for GetSetComplement. */
	template <typename T>
	static TSet<T> GetSetDifference(const TSet<T>& SetA, const TSet<T>& SetB);
};

/**
 * Utility functions for common TMap operations.
 */
struct FVSMap
{
	/** Returns keys present in both maps. */
	template <typename K, typename V>
	static TSet<K> GetKeyIntersection(const TMap<K, V>& MapA, const TMap<K, V>& MapB);

	/** Returns keys present in MapA but not MapB. */
	template <typename K, typename V>
	static TSet<K> GetKeyComplement(const TMap<K, V>& MapA, const TMap<K, V>& MapB);

	/** Copies MapA and overrides with MapB when keys collide. */
	template <typename K, typename V>
	static TMap<K, V> MergeMapsOverride(const TMap<K, V>& MapA, const TMap<K, V>& MapB);

	/** Returns true if both maps contain the same key/value pairs. */
	template <typename K, typename V>
	static bool MapEqual(const TMap<K, V>& MapA, const TMap<K, V>& MapB);
};

/**
 * Utility functions for common TMultiMap operations.
 */
struct FVSMultiMap
{
	/** Returns keys present in both multimaps. */
	template <typename K, typename V>
	static TSet<K> GetKeyIntersection(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB);

	/** Returns keys present in MapA but not MapB. */
	template <typename K, typename V>
	static TSet<K> GetKeyComplement(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB);

	/** Copies MapA and appends all key/value pairs from MapB. */
	template <typename K, typename V>
	static TMultiMap<K, V> MergeMapsAppend(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB);

	/** Returns true if both multimaps contain the same key/value pairs. */
	template <typename K, typename V>
	static bool MapEqual(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB);
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


template <typename T>
TSet<T> FVSSet::GetSetIntersection(const TSet<T>& SetA, const TSet<T>& SetB)
{
	const TSet<T>* Smaller = &SetA;
	const TSet<T>* Larger = &SetB;
	if (SetB.Num() < SetA.Num())
	{
		Smaller = &SetB;
		Larger = &SetA;
	}

	TSet<T> Result;
	for (const T& Elem : *Smaller)
	{
		if (Larger->Contains(Elem))
		{
			Result.Add(Elem);
		}
	}

	return Result;
}

template <typename T>
TSet<T> FVSSet::GetSetUnion(const TSet<T>& SetA, const TSet<T>& SetB)
{
	TSet<T> Result = SetA;
	Result.Append(SetB);
	return Result;
}

template <typename T>
TSet<T> FVSSet::GetSetComplement(const TSet<T>& SetA, const TSet<T>& SetB)
{
	TSet<T> Result;
	for (const T& Elem : SetA)
	{
		if (!SetB.Contains(Elem))
		{
			Result.Add(Elem);
		}
	}

	return Result;
}

template <typename T>
TSet<T> FVSSet::GetSetDifference(const TSet<T>& SetA, const TSet<T>& SetB)
{
	return GetSetComplement(SetA, SetB);
}


template <typename K, typename V>
TSet<K> FVSMap::GetKeyIntersection(const TMap<K, V>& MapA, const TMap<K, V>& MapB)
{
	const TMap<K, V>* Smaller = &MapA;
	const TMap<K, V>* Larger = &MapB;
	if (MapB.Num() < MapA.Num())
	{
		Smaller = &MapB;
		Larger = &MapA;
	}

	TSet<K> Result;
	for (const TPair<K, V>& Pair : *Smaller)
	{
		if (Larger->Contains(Pair.Key))
		{
			Result.Add(Pair.Key);
		}
	}

	return Result;
}

template <typename K, typename V>
TSet<K> FVSMap::GetKeyComplement(const TMap<K, V>& MapA, const TMap<K, V>& MapB)
{
	TSet<K> Result;
	for (const TPair<K, V>& Pair : MapA)
	{
		if (!MapB.Contains(Pair.Key))
		{
			Result.Add(Pair.Key);
		}
	}

	return Result;
}

template <typename K, typename V>
TMap<K, V> FVSMap::MergeMapsOverride(const TMap<K, V>& MapA, const TMap<K, V>& MapB)
{
	TMap<K, V> Result = MapA;
	for (const TPair<K, V>& Pair : MapB)
	{
		Result.Add(Pair.Key, Pair.Value);
	}

	return Result;
}

template <typename K, typename V>
bool FVSMap::MapEqual(const TMap<K, V>& MapA, const TMap<K, V>& MapB)
{
	if (MapA.Num() != MapB.Num())
	{
		return false;
	}

	for (const TPair<K, V>& Pair : MapA)
	{
		const V* OtherValue = MapB.Find(Pair.Key);
		if (!OtherValue || *OtherValue != Pair.Value)
		{
			return false;
		}
	}

	return true;
}


template <typename K, typename V>
TSet<K> FVSMultiMap::GetKeyIntersection(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB)
{
	TArray<K> KeysA;
	TArray<K> KeysB;
	MapA.GetKeys(KeysA);
	MapB.GetKeys(KeysB);

	TSet<K> SetA(KeysA);
	TSet<K> SetB(KeysB);

	const TSet<K>* Smaller = &SetA;
	const TSet<K>* Larger = &SetB;
	if (SetB.Num() < SetA.Num())
	{
		Smaller = &SetB;
		Larger = &SetA;
	}

	TSet<K> Result;
	for (const K& Key : *Smaller)
	{
		if (Larger->Contains(Key))
		{
			Result.Add(Key);
		}
	}

	return Result;
}

template <typename K, typename V>
TSet<K> FVSMultiMap::GetKeyComplement(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB)
{
	TArray<K> KeysA;
	TArray<K> KeysB;
	MapA.GetKeys(KeysA);
	MapB.GetKeys(KeysB);

	TSet<K> SetB(KeysB);
	TSet<K> Result;
	for (const K& Key : KeysA)
	{
		if (!SetB.Contains(Key))
		{
			Result.Add(Key);
		}
	}

	return Result;
}

template <typename K, typename V>
TMultiMap<K, V> FVSMultiMap::MergeMapsAppend(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB)
{
	TMultiMap<K, V> Result = MapA;
	for (const TPair<K, V>& Pair : MapB)
	{
		Result.Add(Pair.Key, Pair.Value);
	}

	return Result;
}

template <typename K, typename V>
bool FVSMultiMap::MapEqual(const TMultiMap<K, V>& MapA, const TMultiMap<K, V>& MapB)
{
	if (MapA.Num() != MapB.Num())
	{
		return false;
	}

	TArray<K> KeysA;
	MapA.GetKeys(KeysA);

	for (const K& Key : KeysA)
	{
		TArray<V> ValuesA;
		TArray<V> ValuesB;
		MapA.MultiFind(Key, ValuesA);
		MapB.MultiFind(Key, ValuesB);

		if (ValuesA.Num() != ValuesB.Num())
		{
			return false;
		}

		for (const V& Value : ValuesA)
		{
			if (ValuesB.RemoveSingle(Value) == 0)
			{
				return false;
			}
		}
	}

	return true;
}
