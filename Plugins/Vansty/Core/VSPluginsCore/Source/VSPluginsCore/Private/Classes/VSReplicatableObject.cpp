// Copyright VanstySanic. All Rights Reserved.

#include "Classes/VSReplicatableObject.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

UVSReplicatableObject::UVSReplicatableObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = false;
}

void UVSReplicatableObject::PostInitProperties()
{
	UObject::PostInitProperties();

	OuterActorPrivate = GetTypedOuter<AActor>();
}

bool UVSReplicatableObject::IsSupportedForNetworking() const
{
	return GetIsReplicated() || IsNameStableForNetworking();
}

int32 UVSReplicatableObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if ((Function->FunctionFlags & FUNC_Static))
	{
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}
	
	return (OuterActorPrivate.IsValid() ? OuterActorPrivate->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool UVSReplicatableObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	bool bProcessed = false;
	if (OuterActorPrivate.IsValid())
	{
		FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
		if (Context != nullptr)
		{
			for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
			{
				if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(OuterActorPrivate.Get(), Function))
				{
					Driver.NetDriver->ProcessRemoteFunction(OuterActorPrivate.Get(), Function, Parms, OutParms, Stack, this);
					bProcessed = true;
				}
			}
		}
	}
	return bProcessed;
}

void UVSReplicatableObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != nullptr) { BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps); }

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UVSReplicatableObject, bReplicates, SharedParams);
}

UWorld* UVSReplicatableObject::GetWorld() const
{
	/** If we are a CDO, we must return nullptr instead of calling Outer->GetWorld() to fool UObject::ImplementsGetWorld. */
	if (GetOuter() && !HasAnyFlags(RF_ClassDefaultObject))
	{
		return GetOuter()->GetWorld();
	}

	return nullptr;
}

void UVSReplicatableObject::OnCreatedFromReplication()
{
	SetIsReplicated(true);
}

void UVSReplicatableObject::OnDestroyedFromReplication()
{
	MarkAsGarbage();
}

void UVSReplicatableObject::SetIsReplicated(bool bShouldReplicate)
{
	if (bReplicates == bShouldReplicate) return;
	bReplicates = bShouldReplicate;

	if (IsTemplate()) return;

	if (OuterActorPrivate.IsValid() && !OuterActorPrivate->HasAnyFlags(RF_NeedInitialization) && OuterActorPrivate->HasAuthority())
	{
		bReplicates ? BeginReplication() : EndReplication();
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(UVSReplicatableObject, bReplicates, this);
}

void UVSReplicatableObject::BeginReplication()
{
#if UE_WITH_IRIS
	UActorComponent* OuterComponent = GetTypedOuter<UActorComponent>();
	if (OuterComponent && OuterComponent->GetIsReplicated() && OuterComponent->IsSupportedForNetworking() && OuterComponent->IsUsingRegisteredSubObjectList())
	{
		OuterComponent->AddReplicatedSubObject(this);
	}
	else if (OuterActorPrivate.IsValid() && OuterActorPrivate->GetIsReplicated() && OuterActorPrivate->IsSupportedForNetworking() && OuterActorPrivate->IsUsingRegisteredSubObjectList())
	{
		OuterActorPrivate->AddReplicatedSubObject(this);
	}
#endif
}

void UVSReplicatableObject::EndReplication()
{
#if UE_WITH_IRIS
	UActorComponent* OuterComponent = GetTypedOuter<UActorComponent>();
	if (OuterComponent && OuterComponent->IsReplicatedSubObjectRegistered(this))
	{
		OuterComponent->RemoveReplicatedSubObject(this);
	}
	else
	{
		OuterActorPrivate->RemoveReplicatedSubObject(this);
	}
#endif
}

