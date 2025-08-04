// Copyright VanstySanic. All Rights Reserved.


#include "Libraries/VSMetaDataLibrary.h"

UVSMetaDataLibrary::UVSMetaDataLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FName UVSMetaDataLibrary::GetClassPropertyMetaData(UClass* Class, FName PropertyName, FName MetaDataName)
{
#if WITH_EDITOR
	if (!Class) return NAME_None;
	FProperty* Prop = Class->FindPropertyByName(PropertyName);
	if (!Prop) return NAME_None;

	if (Prop->HasMetaData(*MetaDataName.ToString()))
	{
		return FName(*Prop->GetMetaData(*MetaDataName.ToString()));
	}
#endif
	
	return NAME_None;
}

FName UVSMetaDataLibrary::GetClassMapKeyPropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName)
{
#if WITH_EDITOR
	if (!Class) return NAME_None;
	FProperty* MapProp = Class->FindPropertyByName(MapPropertyName);
	if (!MapProp || !MapProp->IsA(FMapProperty::StaticClass())) return NAME_None;

	FMapProperty* MapProperty = CastField<FMapProperty>(MapProp);
	FProperty* KeyProp = MapProperty->KeyProp;
	if (!KeyProp) return NAME_None;

	if (KeyProp->HasMetaData(*MetaDataName.ToString()))
	{
		return FName(*KeyProp->GetMetaData(*MetaDataName.ToString()));
	}
#endif
	
	return NAME_None;
}

FName UVSMetaDataLibrary::GetClassMapValuePropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName)
{
#if WITH_EDITOR
	if (!Class) return NAME_None;
	FProperty* MapProp = Class->FindPropertyByName(MapPropertyName);
	if (!MapProp || !MapProp->IsA(FMapProperty::StaticClass())) return NAME_None;

	FMapProperty* MapProperty = CastField<FMapProperty>(MapProp);
	FProperty* ValueProp = MapProperty->ValueProp;
	if (!ValueProp) return NAME_None;

	if (ValueProp->HasMetaData(*MetaDataName.ToString()))
	{
		return FName(*ValueProp->GetMetaData(*MetaDataName.ToString()));
	}
#endif

	return NAME_None;
}

bool UVSMetaDataLibrary::SetClassPropertyMetaData(UClass* Class, FName PropertyName, FName MetaDataName, FName MetaData)
{
#if WITH_EDITOR
	if (!Class) return false;
	FProperty* Prop = Class->FindPropertyByName(PropertyName);
	if (!Prop) return false;

	Prop->SetMetaData(*MetaDataName.ToString(), *MetaData.ToString());

	// FPropertyChangedEvent ChangeEvent(Prop, EPropertyChangeType::ValueSet);
	// FCoreUObjectDelegates::OnObjectPropertyChanged.Broadcast(Object, ChangeEvent);
#endif

	return true;
}

bool UVSMetaDataLibrary::SetClassMapKeyPropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName, FName MetaData)
{
#if WITH_EDITOR
	if (!Class) return false;
	FProperty* MapProp = Class->FindPropertyByName(MapPropertyName);
	if (!MapProp || !MapProp->IsA(FMapProperty::StaticClass())) return false;

	FMapProperty* MapProperty = CastField<FMapProperty>(MapProp);
	FProperty* KeyProp = MapProperty->KeyProp;
	if (!KeyProp) return false;

	KeyProp->SetMetaData(*MetaDataName.ToString(), *MetaData.ToString());

	// FPropertyChangedEvent ChangeEvent(KeyProp, EPropertyChangeType::ValueSet);
	// FCoreUObjectDelegates::OnObjectPropertyChanged.Broadcast(Object, ChangeEvent);
#endif

	return true;
}

bool UVSMetaDataLibrary::SetClassMapValuePropertyMetaData(UClass* Class, FName MapPropertyName, FName MetaDataName, FName MetaData)
{
#if WITH_EDITOR
	if (!Class) return false;
	FProperty* MapProp = Class->FindPropertyByName(MapPropertyName);
	if (!MapProp || !MapProp->IsA(FMapProperty::StaticClass())) return false;
	
	FMapProperty* MapProperty = CastField<FMapProperty>(MapProp);
	FProperty* ValueProp = MapProperty->ValueProp;
	if (!ValueProp) return false;

	ValueProp->SetMetaData(*MetaDataName.ToString(), *MetaData.ToString());

	// FPropertyChangedEvent ChangeEvent(ValueProp, EPropertyChangeType::ValueSet);
	// FCoreUObjectDelegates::OnObjectPropertyChanged.Broadcast(Object, ChangeEvent);
#endif

	return true;
}