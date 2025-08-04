// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSObjectLibrary.h"

UVSObjectLibrary::UVSObjectLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSObjectLibrary::IsValidThreadSafe(const UObject* Object)
{
	return IsValid(Object);
}

bool UVSObjectLibrary::IsObjectInOuter(const UObject* Source, const UObject* PossibleOuterObject)
{
	if (!Source) return false;
	return Source->IsInOuter(PossibleOuterObject);
}

UObject* UVSObjectLibrary::GetTypedOuterObject(const UObject* Object, const TSubclassOf<UObject> Class)
{
	return Object->GetTypedOuter(Class);
}

UObject* UVSObjectLibrary::GetImplementingOuterObject(const UObject* Object, const TSubclassOf<UInterface> Interface)
{
	if (!Object) return nullptr;
	return static_cast<UObject*>(Object->GetImplementingOuterObject(Interface));
}

void UVSObjectLibrary::AddTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject)
{
	FTickFunction* TargetTickFunction = GetTickFunction(Target);
	FTickFunction* PrerequisiteObjectTickFunction = GetTickFunction(PrerequisiteObject);
	if (!TargetTickFunction || !PrerequisiteObjectTickFunction) return;
	TargetTickFunction->AddPrerequisite(PrerequisiteObject, *PrerequisiteObjectTickFunction);
}

void UVSObjectLibrary::RemoveTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject)
{
	FTickFunction* TargetTickFunction = GetTickFunction(Target);
	FTickFunction* PrerequisiteObjectTickFunction = GetTickFunction(PrerequisiteObject);
	if (!TargetTickFunction || !PrerequisiteObjectTickFunction) return;
	TargetTickFunction->RemovePrerequisite(PrerequisiteObject, *PrerequisiteObjectTickFunction);
}

FTickFunction* UVSObjectLibrary::GetTickFunction(UObject* Object)
{
	if (!Object) return nullptr;
	UClass* ObjectClass = Object->GetClass();
	for(FProperty* Property = ObjectClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		if(FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			if (FTickFunction* TickFunction = StructProperty->ContainerPtrToValuePtr<FTickFunction>(Object))
			{
				return TickFunction;
			}
		}
	}

	return nullptr;
}
