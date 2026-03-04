// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSObjectLibrary.h"
#include "Classes/VSObjectFeature.h"
#include "Classes/Components/VSObjectFeatureComponent.h"

UVSObjectLibrary::UVSObjectLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSObjectLibrary::IsObjectTemplate(UObject* Object)
{
	return Object && Object->IsTemplate();
}
bool UVSObjectLibrary::IsValidThreadSafe(const UObject* Object)
{
	return IsValid(Object);
}

bool UVSObjectLibrary::HasOuterObject(const UObject* Source, const UObject* Outer)
{
	return Source ? Source->IsInOuter(Outer) : false;
}

UObject* UVSObjectLibrary::GetTypedOuterObject(const UObject* Object, const TSubclassOf<UObject> Class)
{
	return Object ? Object->GetTypedOuter(Class) : nullptr;
}

UObject* UVSObjectLibrary::GetImplementingOuterObject(const UObject* Object, const TSubclassOf<UInterface> Interface)
{
	return Object ? static_cast<UObject*>(Object->GetImplementingOuterObject(Interface)) : nullptr;
}

void UVSObjectLibrary::AddTickPrerequisiteObject(UObject* Source, UObject* PrerequisiteObject)
{
	if (!Source || !PrerequisiteObject || Source == PrerequisiteObject) return;
	FTickFunction* TargetTickFunction = GetTickFunctionPtrFromObject(Source);
	FTickFunction* PrerequisiteObjectTickFunction = GetTickFunctionPtrFromObject(PrerequisiteObject);
	if (TargetTickFunction == PrerequisiteObjectTickFunction) return;
	if (!TargetTickFunction || !PrerequisiteObjectTickFunction) return;
	TargetTickFunction->AddPrerequisite(PrerequisiteObject, *PrerequisiteObjectTickFunction);
}

void UVSObjectLibrary::RemoveTickPrerequisiteObject(UObject* Source, UObject* PrerequisiteObject)
{
	if (!Source || !PrerequisiteObject || Source == PrerequisiteObject) return;
	FTickFunction* TargetTickFunction = GetTickFunctionPtrFromObject(Source);
	FTickFunction* PrerequisiteObjectTickFunction = GetTickFunctionPtrFromObject(PrerequisiteObject);
	if (TargetTickFunction == PrerequisiteObjectTickFunction) return;
	if (!TargetTickFunction || !PrerequisiteObjectTickFunction) return;
	TargetTickFunction->RemovePrerequisite(PrerequisiteObject, *PrerequisiteObjectTickFunction);
}

UVSObjectFeature* UVSObjectLibrary::GetFeatureByClassFromObject(UObject* Object, TSubclassOf<UVSObjectFeature> Class)
{
	if (!Object || !Class) return nullptr;

	if (AActor* Actor = Cast<AActor>(Object))
	{
		TArray<UActorComponent*> Components;
		Actor->GetComponents(UVSFeatureComponent::StaticClass(), Components);
		for (UActorComponent* Component : Components)
		{
			if (UVSFeatureComponent* FeatureComponent = Cast<UVSFeatureComponent>(Component))
			{
				if (UVSObjectFeature* Feature = FeatureComponent->GetFeatureByClass(Class))
				{
					return Feature;
				}
			}
		}
	}

	if (UVSFeatureComponent* FeatureComponent = Cast<UVSFeatureComponent>(Object))
	{
		if (UVSObjectFeature* Feature = FeatureComponent->GetFeatureByClass(Class))
		{
			return Feature;
		}
	}

	/** Find in all direct properties. */
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
		Actor->GetComponents(UVSFeatureComponent::StaticClass(), Components);
		for (UActorComponent* Component : Components)
		{
			if (UVSFeatureComponent* FeatureComponent = Cast<UVSFeatureComponent>(Component))
			{
				if (UVSObjectFeature* Feature = FeatureComponent->GetFeatureByName(Name))
				{
					return Feature;
				}
			}
		}
	}
	
	if (UVSFeatureComponent* FeatureComponent = Cast<UVSFeatureComponent>(Object))
	{
		if (UVSObjectFeature* Feature = FeatureComponent->GetFeatureByName(Name))
		{
			return Feature;
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

FString UVSObjectLibrary::ExportPropertyToText(UObject* Object, FName PropertyName)
{
	if (!IsValid(Object)) return FString();

	FProperty* Property = FindFProperty<FProperty>(Object->GetClass(), PropertyName);
	if (!Property) return FString();
	
	void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);

	FString OutString;
	Property->ExportText_Direct(
		OutString,
		ValuePtr,
		nullptr,
		Object,
		PPF_None
	);

	return OutString;
}

bool UVSObjectLibrary::ImportPropertyFromText(UObject* Object, FName PropertyName, const FString& Text)
{
	if (!IsValid(Object)) return false;

	FProperty* Property = FindFProperty<FProperty>(Object->GetClass(), PropertyName);
	if (!Property) return false;
	void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);

	return Property->ImportText_Direct(
		*Text,
		ValuePtr,
		Object,
		PPF_None
	) != nullptr;
}


FTickFunction* UVSObjectLibrary::GetTickFunctionPtrFromObject(UObject* Object)
{
	if (!Object) return nullptr;

	/** Get from ordinary types. */
	if (AActor* Actor = Cast<AActor>(Object))
	{
		return &Actor->PrimaryActorTick;
	}
	if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
	{
		return &ActorComponent->PrimaryComponentTick;
	}
	if (IVSTickFunctionOwnerInterface* TickFunctionInterface = Cast<IVSTickFunctionOwnerInterface>(Object))
	{
		return TickFunctionInterface->GetPrimaryTickFunctionPtr();
	}

	/** Find a tick function in properties. */
	for(FProperty* Property = Object->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		if(FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			UScriptStruct* StructType = StructProperty->Struct;
			if (StructType && StructType->IsChildOf(TBaseStructure<FTickFunction>::Get()))
			{
				if (FTickFunction* TickFunction = StructProperty->ContainerPtrToValuePtr<FTickFunction>(Object))
				{
					return TickFunction;
				}
			}
		}
	}
	
	return nullptr;
}
