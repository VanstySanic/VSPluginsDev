// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSContainerLibrary.h"
#include "UObject/UnrealType.h"

UVSContainerLibrary::UVSContainerLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

namespace VSContainerLibraryPrivate
{
	static bool AreArrayTypesCompatible(const FArrayProperty* A, const FArrayProperty* B, const FArrayProperty* Result)
	{
		if (!A || !B || !Result || !A->Inner || !B->Inner || !Result->Inner)
		{
			return false;
		}

		return A->Inner->SameType(B->Inner) && A->Inner->SameType(Result->Inner);
	}

	static bool ArrayContains(FScriptArrayHelper& Helper, const FProperty* Inner, const void* ElementPtr)
	{
		for (int32 Index = 0; Index < Helper.Num(); ++Index)
		{
			if (Inner->Identical(Helper.GetRawPtr(Index), ElementPtr))
			{
				return true;
			}
		}
		return false;
	}

	static void ArrayAddUnique(FScriptArrayHelper& Helper, const FProperty* Inner, const void* ElementPtr)
	{
		if (ArrayContains(Helper, Inner, ElementPtr))
		{
			return;
		}

		const int32 NewIndex = Helper.AddValue();
		Inner->CopyCompleteValue(Helper.GetRawPtr(NewIndex), ElementPtr);
	}

	static bool AreSetTypesCompatible(const FSetProperty* A, const FSetProperty* B, const FSetProperty* Result)
	{
		if (!A || !B || !Result || !A->ElementProp || !B->ElementProp || !Result->ElementProp)
		{
			return false;
		}

		return A->ElementProp->SameType(B->ElementProp) && A->ElementProp->SameType(Result->ElementProp);
	}

	static bool SetContains(const FScriptSetHelper& Helper, const FProperty* ElementProp, const void* ElementPtr)
	{
		for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index)
		{
			if (!Helper.IsValidIndex(Index))
			{
				continue;
			}

			if (ElementProp->Identical(Helper.GetElementPtr(Index), ElementPtr))
			{
				return true;
			}
		}

		return false;
	}

	static void SetAddUnique(FScriptSetHelper& Helper, const FProperty* ElementProp, const void* ElementPtr)
	{
		if (SetContains(Helper, ElementProp, ElementPtr))
		{
			return;
		}

		const int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();
		ElementProp->CopyCompleteValue(Helper.GetElementPtr(NewIndex), ElementPtr);
	}

	static bool AreMapTypesCompatible(const FMapProperty* A, const FMapProperty* B, const FMapProperty* Result)
	{
		if (!A || !B || !Result || !A->KeyProp || !A->ValueProp || !B->KeyProp || !B->ValueProp || !Result->KeyProp || !Result->ValueProp)
		{
			return false;
		}

		return A->KeyProp->SameType(B->KeyProp) && A->ValueProp->SameType(B->ValueProp)
			&& A->KeyProp->SameType(Result->KeyProp) && A->ValueProp->SameType(Result->ValueProp);
	}

	static bool AreMapKeySetCompatible(const FMapProperty* MapProp, const FSetProperty* SetProp)
	{
		if (!MapProp || !SetProp || !MapProp->KeyProp || !SetProp->ElementProp)
		{
			return false;
		}

		return MapProp->KeyProp->SameType(SetProp->ElementProp);
	}

	static int32 MapFindKeyIndex(FScriptMapHelper& Helper, const FProperty* KeyProp, const void* KeyPtr)
	{
		for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index)
		{
			if (!Helper.IsValidIndex(Index))
			{
				continue;
			}

			if (KeyProp->Identical(Helper.GetKeyPtr(Index), KeyPtr))
			{
				return Index;
			}
		}

		return INDEX_NONE;
	}

	static bool MapContainsKey(FScriptMapHelper& Helper, const FProperty* KeyProp, const void* KeyPtr)
	{
		return MapFindKeyIndex(Helper, KeyProp, KeyPtr) != INDEX_NONE;
	}

	static void MapAddOrOverride(FScriptMapHelper& Helper, const FProperty* KeyProp, const FProperty* ValueProp, const void* KeyPtr, const void* ValuePtr)
	{
		const int32 ExistingIndex = MapFindKeyIndex(Helper, KeyProp, KeyPtr);
		if (ExistingIndex != INDEX_NONE)
		{
			ValueProp->CopyCompleteValue(Helper.GetValuePtr(ExistingIndex), ValuePtr);
			return;
		}

		const int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();
		KeyProp->CopyCompleteValue(Helper.GetKeyPtr(NewIndex), KeyPtr);
		ValueProp->CopyCompleteValue(Helper.GetValuePtr(NewIndex), ValuePtr);
	}
}



void UVSContainerLibrary::ArrayIntersection(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result)
{
}

void UVSContainerLibrary::ArrayUnion(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result)
{
}

void UVSContainerLibrary::ArrayComplement(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result)
{
}

void UVSContainerLibrary::ArrayDifference(const TArray<int32>& ArrayA, const TArray<int32>& ArrayB, TArray<int32>& Result)
{
}

void UVSContainerLibrary::ArrayRemoveDuplicates(TArray<int32>& Array)
{
}


void UVSContainerLibrary::SetIntersection(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result)
{
}

void UVSContainerLibrary::SetUnion(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result)
{
}

void UVSContainerLibrary::SetComplement(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result)
{
}

void UVSContainerLibrary::SetDifference(const TSet<int32>& SetA, const TSet<int32>& SetB, TSet<int32>& Result)
{
}


void UVSContainerLibrary::MapKeyIntersection(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB, TSet<int32>& ResultKeys)
{
}

void UVSContainerLibrary::MapKeyComplement(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB, TSet<int32>& ResultKeys)
{
}

void UVSContainerLibrary::MapMergeOverride(const TMap<int32, int32>& MapA, const TMap<int32, int32>& MapB, TMap<int32, int32>& Result)
{
}



DEFINE_FUNCTION(UVSContainerLibrary::execArrayIntersection)
{
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayAAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayAProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayBAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayBProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ResultProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreArrayTypesCompatible(ArrayAProp, ArrayBProp, ResultProp))
	{
		return;
	}

	const FProperty* Inner = ArrayAProp->Inner;
	FScriptArrayHelper ArrayAHelper(ArrayAProp, ArrayAAddr);
	FScriptArrayHelper ArrayBHelper(ArrayBProp, ArrayBAddr);
	FScriptArrayHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyValues();

	for (int32 Index = 0; Index < ArrayAHelper.Num(); ++Index)
	{
		const void* ElementPtr = ArrayAHelper.GetRawPtr(Index);
		if (VSContainerLibraryPrivate::ArrayContains(ArrayBHelper, Inner, ElementPtr))
		{
			VSContainerLibraryPrivate::ArrayAddUnique(ResultHelper, Inner, ElementPtr);
		}
	}
}

DEFINE_FUNCTION(UVSContainerLibrary::execArrayUnion)
{
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayAAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayAProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayBAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayBProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ResultProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreArrayTypesCompatible(ArrayAProp, ArrayBProp, ResultProp))
	{
		return;
	}

	const FProperty* Inner = ArrayAProp->Inner;
	FScriptArrayHelper ArrayAHelper(ArrayAProp, ArrayAAddr);
	FScriptArrayHelper ArrayBHelper(ArrayBProp, ArrayBAddr);
	FScriptArrayHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyValues();

	for (int32 Index = 0; Index < ArrayAHelper.Num(); ++Index)
	{
		VSContainerLibraryPrivate::ArrayAddUnique(ResultHelper, Inner, ArrayAHelper.GetRawPtr(Index));
	}

	for (int32 Index = 0; Index < ArrayBHelper.Num(); ++Index)
	{
		VSContainerLibraryPrivate::ArrayAddUnique(ResultHelper, Inner, ArrayBHelper.GetRawPtr(Index));
	}
}

DEFINE_FUNCTION(UVSContainerLibrary::execArrayComplement)
{
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayAAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayAProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayBAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayBProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ResultProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreArrayTypesCompatible(ArrayAProp, ArrayBProp, ResultProp))
	{
		return;
	}

	const FProperty* Inner = ArrayAProp->Inner;
	FScriptArrayHelper ArrayAHelper(ArrayAProp, ArrayAAddr);
	FScriptArrayHelper ArrayBHelper(ArrayBProp, ArrayBAddr);
	FScriptArrayHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyValues();

	for (int32 Index = 0; Index < ArrayAHelper.Num(); ++Index)
	{
		const void* ElementPtr = ArrayAHelper.GetRawPtr(Index);
		if (!VSContainerLibraryPrivate::ArrayContains(ArrayBHelper, Inner, ElementPtr))
		{
			VSContainerLibraryPrivate::ArrayAddUnique(ResultHelper, Inner, ElementPtr);
		}
	}
}

DEFINE_FUNCTION(UVSContainerLibrary::execArrayRemoveDuplicates)
{
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!ArrayProp || !ArrayProp->Inner || !ArrayAddr)
	{
		return;
	}

	const FProperty* Inner = ArrayProp->Inner;
	FScriptArrayHelper ArrayHelper(ArrayProp, ArrayAddr);
	FScriptArray TempArray;
	FScriptArrayHelper TempHelper(ArrayProp, &TempArray);
	TempHelper.EmptyValues();

	for (int32 Index = 0; Index < ArrayHelper.Num(); ++Index)
	{
		const void* ElementPtr = ArrayHelper.GetRawPtr(Index);
		VSContainerLibraryPrivate::ArrayAddUnique(TempHelper, Inner, ElementPtr);
	}

	ArrayHelper.EmptyValues();
	for (int32 Index = 0; Index < TempHelper.Num(); ++Index)
	{
		const int32 NewIndex = ArrayHelper.AddValue();
		Inner->CopyCompleteValue(ArrayHelper.GetRawPtr(NewIndex), TempHelper.GetRawPtr(Index));
	}
}

DEFINE_FUNCTION(UVSContainerLibrary::execArrayDifference)
{
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayAAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayAProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ArrayBAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ArrayBProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FArrayProperty* ResultProp = CastField<FArrayProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreArrayTypesCompatible(ArrayAProp, ArrayBProp, ResultProp))
	{
		return;
	}

	const FProperty* Inner = ArrayAProp->Inner;
	FScriptArrayHelper ArrayAHelper(ArrayAProp, ArrayAAddr);
	FScriptArrayHelper ArrayBHelper(ArrayBProp, ArrayBAddr);
	FScriptArrayHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyValues();

	for (int32 Index = 0; Index < ArrayAHelper.Num(); ++Index)
	{
		const void* ElementPtr = ArrayAHelper.GetRawPtr(Index);
		if (!VSContainerLibraryPrivate::ArrayContains(ArrayBHelper, Inner, ElementPtr))
		{
			VSContainerLibraryPrivate::ArrayAddUnique(ResultHelper, Inner, ElementPtr);
		}
	}
}


DEFINE_FUNCTION(UVSContainerLibrary::execSetIntersection)
{
	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetAAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetAProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetBAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetBProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* ResultProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreSetTypesCompatible(SetAProp, SetBProp, ResultProp))
	{
		return;
	}

	const FProperty* ElementProp = SetAProp->ElementProp;
	FScriptSetHelper SetAHelper(SetAProp, SetAAddr);
	FScriptSetHelper SetBHelper(SetBProp, SetBAddr);
	FScriptSetHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyElements();

	for (int32 Index = 0; Index < SetAHelper.GetMaxIndex(); ++Index)
	{
		if (!SetAHelper.IsValidIndex(Index))
		{
			continue;
		}

		const void* ElementPtr = SetAHelper.GetElementPtr(Index);
		if (VSContainerLibraryPrivate::SetContains(SetBHelper, ElementProp, ElementPtr))
		{
			VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ElementProp, ElementPtr);
		}
	}

	ResultHelper.Rehash();
}

DEFINE_FUNCTION(UVSContainerLibrary::execSetUnion)
{
	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetAAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetAProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetBAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetBProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* ResultProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreSetTypesCompatible(SetAProp, SetBProp, ResultProp))
	{
		return;
	}

	const FProperty* ElementProp = SetAProp->ElementProp;
	FScriptSetHelper SetAHelper(SetAProp, SetAAddr);
	FScriptSetHelper SetBHelper(SetBProp, SetBAddr);
	FScriptSetHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyElements();

	for (int32 Index = 0; Index < SetAHelper.GetMaxIndex(); ++Index)
	{
		if (!SetAHelper.IsValidIndex(Index))
		{
			continue;
		}

		VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ElementProp, SetAHelper.GetElementPtr(Index));
	}

	for (int32 Index = 0; Index < SetBHelper.GetMaxIndex(); ++Index)
	{
		if (!SetBHelper.IsValidIndex(Index))
		{
			continue;
		}

		VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ElementProp, SetBHelper.GetElementPtr(Index));
	}

	ResultHelper.Rehash();
}

DEFINE_FUNCTION(UVSContainerLibrary::execSetComplement)
{
	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetAAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetAProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetBAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetBProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* ResultProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreSetTypesCompatible(SetAProp, SetBProp, ResultProp))
	{
		return;
	}

	const FProperty* ElementProp = SetAProp->ElementProp;
	FScriptSetHelper SetAHelper(SetAProp, SetAAddr);
	FScriptSetHelper SetBHelper(SetBProp, SetBAddr);
	FScriptSetHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyElements();

	for (int32 Index = 0; Index < SetAHelper.GetMaxIndex(); ++Index)
	{
		if (!SetAHelper.IsValidIndex(Index))
		{
			continue;
		}

		const void* ElementPtr = SetAHelper.GetElementPtr(Index);
		if (!VSContainerLibraryPrivate::SetContains(SetBHelper, ElementProp, ElementPtr))
		{
			VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ElementProp, ElementPtr);
		}
	}

	ResultHelper.Rehash();
}

DEFINE_FUNCTION(UVSContainerLibrary::execSetDifference)
{
	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetAAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetAProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* SetBAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* SetBProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* ResultProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreSetTypesCompatible(SetAProp, SetBProp, ResultProp))
	{
		return;
	}

	const FProperty* ElementProp = SetAProp->ElementProp;
	FScriptSetHelper SetAHelper(SetAProp, SetAAddr);
	FScriptSetHelper SetBHelper(SetBProp, SetBAddr);
	FScriptSetHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyElements();

	for (int32 Index = 0; Index < SetAHelper.GetMaxIndex(); ++Index)
	{
		if (!SetAHelper.IsValidIndex(Index))
		{
			continue;
		}

		const void* ElementPtr = SetAHelper.GetElementPtr(Index);
		if (!VSContainerLibraryPrivate::SetContains(SetBHelper, ElementProp, ElementPtr))
		{
			VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ElementProp, ElementPtr);
		}
	}

	ResultHelper.Rehash();
}


DEFINE_FUNCTION(UVSContainerLibrary::execMapKeyIntersection)
{
	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* MapAAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* MapAProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* MapBAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* MapBProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* ResultProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreMapKeySetCompatible(MapAProp, ResultProp) || !VSContainerLibraryPrivate::AreMapKeySetCompatible(MapBProp, ResultProp))
	{
		return;
	}

	if (!MapAProp || !MapBProp)
	{
		return;
	}

	const FProperty* KeyProp = MapAProp->KeyProp;
	FScriptMapHelper MapAHelper(MapAProp, MapAAddr);
	FScriptMapHelper MapBHelper(MapBProp, MapBAddr);
	FScriptSetHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyElements();

	for (int32 Index = 0; Index < MapAHelper.GetMaxIndex(); ++Index)
	{
		if (!MapAHelper.IsValidIndex(Index))
		{
			continue;
		}

		const void* KeyPtr = MapAHelper.GetKeyPtr(Index);
		if (VSContainerLibraryPrivate::MapContainsKey(MapBHelper, KeyProp, KeyPtr))
		{
			VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ResultProp->ElementProp, KeyPtr);
		}
	}

	ResultHelper.Rehash();
}

DEFINE_FUNCTION(UVSContainerLibrary::execMapKeyComplement)
{
	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* MapAAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* MapAProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* MapBAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* MapBProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FSetProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FSetProperty* ResultProp = CastField<FSetProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreMapKeySetCompatible(MapAProp, ResultProp) || !VSContainerLibraryPrivate::AreMapKeySetCompatible(MapBProp, ResultProp))
	{
		return;
	}

	if (!MapAProp || !MapBProp)
	{
		return;
	}

	const FProperty* KeyProp = MapAProp->KeyProp;
	FScriptMapHelper MapAHelper(MapAProp, MapAAddr);
	FScriptMapHelper MapBHelper(MapBProp, MapBAddr);
	FScriptSetHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyElements();

	for (int32 Index = 0; Index < MapAHelper.GetMaxIndex(); ++Index)
	{
		if (!MapAHelper.IsValidIndex(Index))
		{
			continue;
		}

		const void* KeyPtr = MapAHelper.GetKeyPtr(Index);
		if (!VSContainerLibraryPrivate::MapContainsKey(MapBHelper, KeyProp, KeyPtr))
		{
			VSContainerLibraryPrivate::SetAddUnique(ResultHelper, ResultProp->ElementProp, KeyPtr);
		}
	}

	ResultHelper.Rehash();
}

DEFINE_FUNCTION(UVSContainerLibrary::execMapMergeOverride)
{
	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* MapAAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* MapAProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* MapBAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* MapBProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	Stack.StepCompiledIn<FMapProperty>(nullptr);
	void* ResultAddr = Stack.MostRecentPropertyAddress;
	FMapProperty* ResultProp = CastField<FMapProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (!VSContainerLibraryPrivate::AreMapTypesCompatible(MapAProp, MapBProp, ResultProp))
	{
		return;
	}

	const FProperty* KeyProp = MapAProp->KeyProp;
	const FProperty* ValueProp = MapAProp->ValueProp;
	FScriptMapHelper MapAHelper(MapAProp, MapAAddr);
	FScriptMapHelper MapBHelper(MapBProp, MapBAddr);
	FScriptMapHelper ResultHelper(ResultProp, ResultAddr);
	ResultHelper.EmptyValues();

	for (int32 Index = 0; Index < MapAHelper.GetMaxIndex(); ++Index)
	{
		if (!MapAHelper.IsValidIndex(Index))
		{
			continue;
		}

		VSContainerLibraryPrivate::MapAddOrOverride(ResultHelper, KeyProp, ValueProp, MapAHelper.GetKeyPtr(Index), MapAHelper.GetValuePtr(Index));
	}

	for (int32 Index = 0; Index < MapBHelper.GetMaxIndex(); ++Index)
	{
		if (!MapBHelper.IsValidIndex(Index))
		{
			continue;
		}

		VSContainerLibraryPrivate::MapAddOrOverride(ResultHelper, KeyProp, ValueProp, MapBHelper.GetKeyPtr(Index), MapBHelper.GetValuePtr(Index));
	}

	ResultHelper.Rehash();
}
