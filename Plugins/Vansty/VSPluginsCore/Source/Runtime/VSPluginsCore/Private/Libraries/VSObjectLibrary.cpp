// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSObjectLibrary.h"

#include "Interfaces/VSTickFunctionInterface.h"

UVSObjectLibrary::UVSObjectLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSObjectLibrary::IsObjectWorldRelated(const UObject* InObject)
{
	if (!InObject) return false;
	if (InObject->IsA<AActor>() || InObject->IsA<UActorComponent>() || InObject->IsA<UWorldSubsystem>()) return true;
	
	return IsObjectWorldRelated(InObject->GetOuter());
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
	FTickFunction* TargetTickFunction = GetTickFunctionFromObject(Target);
	FTickFunction* PrerequisiteObjectTickFunction = GetTickFunctionFromObject(PrerequisiteObject);
	if (!TargetTickFunction || !PrerequisiteObjectTickFunction) return;
	TargetTickFunction->AddPrerequisite(PrerequisiteObject, *PrerequisiteObjectTickFunction);
}

void UVSObjectLibrary::RemoveTickPrerequisiteObject(UObject* Target, UObject* PrerequisiteObject)
{
	FTickFunction* TargetTickFunction = GetTickFunctionFromObject(Target);
	FTickFunction* PrerequisiteObjectTickFunction = GetTickFunctionFromObject(PrerequisiteObject);
	if (!TargetTickFunction || !PrerequisiteObjectTickFunction) return;
	TargetTickFunction->RemovePrerequisite(PrerequisiteObject, *PrerequisiteObjectTickFunction);
}

FTickFunction* UVSObjectLibrary::GetTickFunctionFromObject(UObject* Object)
{
	if (!Object) return nullptr;
	if (AActor* Actor = Cast<AActor>(Object))
	{
		return &Actor->PrimaryActorTick;
	}
	if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
	{
		return &ActorComponent->PrimaryComponentTick;
	}
	if (IVSTickFunctionInterface* TickFunctionInterface = Cast<IVSTickFunctionInterface>(Object))
	{
		return TickFunctionInterface->GetTickFunctionPtr();
	}
	
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
