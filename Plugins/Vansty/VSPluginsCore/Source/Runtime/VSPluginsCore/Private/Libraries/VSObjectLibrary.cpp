// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSObjectLibrary.h"

#include "Classes/Features/VSObjectFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
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

UVSObjectFeature* UVSObjectLibrary::GetFeatureByClassFromObject(UObject* Object, TSubclassOf<UVSObjectFeature> Class)
{
	if (!Object || !Class) return nullptr;

	if (AActor* Actor = Cast<AActor>(Object))
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(UVSObjectFeatureComponent::StaticClass(), Components);
		for (UActorComponent* Component : Components)
		{
			if (UVSObjectFeatureComponent* FeatureComponent = Cast<UVSObjectFeatureComponent>(Component))
			{
				if (UVSObjectFeature* Feature = FeatureComponent->GetSubFeatureByClass(Class))
				{
					return Feature;
				}
			}
		}
	}
	
	for (FProperty* Property = Object->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
		if (!ObjectProperty) continue;
		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
		if (!ValuePtr) continue;
		UObject* Value = ObjectProperty->GetPropertyValue(ValuePtr);
		if (!Value) continue;
		if (UVSObjectFeature* Feature = Cast<UVSObjectFeature>(Value))
		{
			if (Value->IsA(Class))
			{
				return Feature;
			}
			if (UVSObjectFeature* SubFeature = Feature->GetSubFeatureByClass(Class))
			{
				return SubFeature;
			}
		}
	}
	
	return nullptr;
}

UVSObjectFeature* UVSObjectLibrary::GetFeatureByNameFromObject(UObject* Object, FName Name)
{
	if (!Object || Name.IsNone()) return nullptr;

	if (AActor* Actor = Cast<AActor>(Object))
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(UVSObjectFeature::StaticClass(), Components);
		for (UActorComponent* Component : Components)
		{
			if (UVSObjectFeatureComponent* FeatureComponent = Cast<UVSObjectFeatureComponent>(Component))
			{
				if (UVSObjectFeature* Feature = FeatureComponent->GetSubFeatureByName(Name))
				{
					return Feature;
				}
			}
		}
	}
	
	for (FProperty* Property = Object->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
		if (!ObjectProperty) continue;
		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
		if (!ValuePtr) continue;
		UObject* Value = ObjectProperty->GetPropertyValue(ValuePtr);
		if (!Value) continue;
		if (UVSObjectFeature* Feature = Cast<UVSObjectFeature>(Value))
		{
			if (Feature->FeatureName == Name)
			{
				return Feature;
			}
			if (UVSObjectFeature* SubFeature = Feature->GetSubFeatureByName(Name))
			{
				return SubFeature;
			}
		}
	}
	
	return nullptr;
}