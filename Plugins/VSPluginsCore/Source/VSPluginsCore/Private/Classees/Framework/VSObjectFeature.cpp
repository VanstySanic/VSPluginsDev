// Copyright VanstySanic. All Rights Reserved.

#include "Classees/Framework/VSObjectFeature.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/Iris/ReplicationSystem/ReplicationSystemUtil.h"

DEFINE_LOG_CATEGORY(LogObjectFeature);

UVSObjectFeature::UVSObjectFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsActive(false), bHasBeenInitialized(false), bHasBegunPlay(false), bIsBeingDestroyed(false)
{
	OwnerActorPrivate = GetOwnerActor();
	OwnerComponentPrivate = GetOwnerComponent();
	OwnerFeaturePrivate = GetOwnerFeature();
}

void UVSObjectFeature::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject) && GetWorld() && !GetWorld()->IsGameWorld())
	{
		RegisterFeature();
	}
}

bool UVSObjectFeature::IsSupportedForNetworking() const
{
	return bReplicates;
}

void UVSObjectFeature::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != nullptr) { BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps); }

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UVSObjectFeature, bIsActive, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UVSObjectFeature, bReplicates, SharedParams);
}

void UVSObjectFeature::BeginDestroy()
{
	TArray<TObjectPtr<UVSObjectFeature>> SubFeaturesToDestroy = SubFeatures;
	for (TObjectPtr<UVSObjectFeature> SubFeature : SubFeaturesToDestroy)
	{
		if (SubFeature && !SubFeature->bIsBeingDestroyed)
		{
			SubFeature->DestroyFeature();
		}
	}
	
	if (bHasBegunPlay) { EndPlay(); }
	if (bHasBeenInitialized) { Uninitialize(); }
	if (bIsRegistered) { UnregisterFeature(); }

	if (UVSObjectFeature* OwnerFeature = GetOwnerFeature()) { OwnerFeature->RemoveSubFeature(this); }
	
	Super::BeginDestroy();
}

int32 UVSObjectFeature::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if ((Function->FunctionFlags & FUNC_Static))
	{
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}

	AActor* MyOwner = GetOwnerActor();
	return (MyOwner ? MyOwner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool UVSObjectFeature::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	bool bProcessed = false;
	if (AActor* MyOwner = GetOwnerActor())
	{
		FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
		if (Context != nullptr)
		{
			for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
			{
				if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(MyOwner, Function))
				{
					Driver.NetDriver->ProcessRemoteFunction(MyOwner, Function, Parms, OutParms, Stack, this);
					bProcessed = true;
				}
			}
		}
	}
	return bProcessed;
}

bool UVSObjectFeature::Modify(bool bAlwaysMarkDirty)
{
	AActor* OwnerActor = GetOwnerActor();    
	bAlwaysMarkDirty = bAlwaysMarkDirty && (!OwnerActor || !OwnerActor->HasAnyFlags(RF_Transient));
	
	if (OwnerActor)
	{
		return OwnerActor->Modify(bAlwaysMarkDirty);
	}

	return Super::Modify(bAlwaysMarkDirty);
}

void UVSObjectFeature::OnCreatedFromReplication()
{
	SetIsReplicated(true);
	if (UVSObjectFeature* OwnerFeature = GetOwnerFeature())
	{
		OwnerFeature->AddSubFeatureInternal(this);
	}
	else
	{
		RegisterFeature();
	}
}

void UVSObjectFeature::OnDestroyedFromReplication()
{
	DestroyFeature();
}

AActor* UVSObjectFeature::GetOwnerActor() const
{
	if (OwnerActorPrivate.IsValid()) return OwnerActorPrivate.Get();
	return GetTypedOuter<AActor>();
}

UActorComponent* UVSObjectFeature::GetOwnerComponent() const
{
	if (OwnerComponentPrivate.IsValid()) return OwnerComponentPrivate.Get();
	return GetTypedOuter<UActorComponent>();
}

UVSObjectFeature* UVSObjectFeature::GetOwnerFeature() const
{
	if (OwnerFeaturePrivate.IsValid()) return OwnerFeaturePrivate.Get();
	return GetTypedOuter<UVSObjectFeature>();
}

void UVSObjectFeature::RegisterFeature()
{
	if(!IsValid(this))
	{
		UE_LOG(LogObjectFeature, Log, TEXT("RegisterFeature: (%s) Trying to register feature with IsValid() == false. Aborting."), *GetPathName());
		return;
	}

	if(IsRegistered())
	{
		UE_LOG(LogObjectFeature, Log, TEXT("RegisterFeature: (%s) Already registered. Aborting."), *GetPathName());
		return;
	}
	
	bIsRegistered = true;

	
	/**
	 * If not in a game world register ticks now, otherwise defer until BeginPlay.
	 * If no owner we won't trigger BeginPlay either so register now in that case as well.
	 */
	if (GetWorld() && !GetWorld()->IsGameWorld())
	{
		RegisterTickFunction();
	}
	else
	{
		AActor* OwnerActor = GetOwnerActor();
		UActorComponent* OwnerComponent = GetOwnerComponent();
		UVSObjectFeature* OwnerFeature = GetOwnerFeature();
		check(OwnerActor);
		
		const bool bOwnerActorBeginPlayStarted = OwnerActor ? (OwnerActor->HasActorBegunPlay() || OwnerActor->IsActorBeginningPlay()) : true;
		const bool bOwnerComponentBeginPlayStarted = OwnerComponent ? (OwnerComponent->HasBeenInitialized() || OwnerComponent->HasBegunPlay()) : true;
		const bool bOwnerFeatureBeginPlayStarted = OwnerFeature ? (OwnerFeature->HasBeenInitialized() || OwnerFeature->HasBegunPlay()) : true;
		
		if (OwnerActor->IsActorInitialized() && bReplicates)
		{
			BeginReplication();
		}
		
		if (bOwnerActorBeginPlayStarted)
		{
			RegisterTickFunction();
		}

		if (bOwnerActorBeginPlayStarted && bOwnerComponentBeginPlayStarted && bOwnerFeatureBeginPlayStarted)
		{
			if (!HasBeenInitialized()) { Initialize(); }
			if (!HasBegunPlay())
			{
				BeginPlay();
				SetActive(bAutoActivate);
			}
		}
	}

	/** Register sub features. */
	for (TObjectPtr<UVSObjectFeature> SubFeature : SubFeatures)
	{
		if (SubFeature && !SubFeature->IsRegistered())
		{
			SubFeature->RegisterFeature();
		}
	}
}

void UVSObjectFeature::UnregisterFeature()
{
	if(!IsValid(this))
	{
		UE_LOG(LogObjectFeature, Log, TEXT("UnregisterFeature: (%s) Trying to unregister feature with IsValid() == false. Aborting."), *GetPathName());
		return;
	}
	
	if(!IsRegistered())
	{
		UE_LOG(LogObjectFeature, Log, TEXT("UnregisterFeature: (%s) Not registered. Aborting."), *GetPathName());
		return;
	}

	bIsRegistered = false;
	
	/** Register sub features. */
	for (TObjectPtr<UVSObjectFeature> SubFeature : SubFeatures)
	{
		if (SubFeature && SubFeature->IsRegistered())
		{
			SubFeature->UnregisterFeature();
		}
	}
	
	if (bHasBegunPlay) { EndPlay(); }
	if (bHasBeenInitialized) { Uninitialize(); }
	if (bReplicates) { EndReplication(); }
	
	UnregisterFeature();
}

void UVSObjectFeature::Initialize_Implementation()
{
	if (bHasBeenInitialized) return;
	bHasBeenInitialized = true;
}

void UVSObjectFeature::Uninitialize_Implementation()
{
	if (!bHasBeenInitialized) return;
	bHasBeenInitialized = false;
}

void UVSObjectFeature::BeginPlay_Implementation()
{
	if (bHasBegunPlay) return;
	bHasBegunPlay = true;
}

void UVSObjectFeature::EndPlay_Implementation()
{
	if (!bHasBegunPlay) return;
	bHasBegunPlay = false;
}

void UVSObjectFeature::DestroyFeature()
{
	TArray<TObjectPtr<UVSObjectFeature>> SubFeaturesToDestroy = SubFeatures;
	for (TObjectPtr<UVSObjectFeature> SubFeature : SubFeaturesToDestroy)
	{
		if (SubFeature)
		{
			SubFeature->DestroyFeature();
		}
	}
	
	if (bIsBeingDestroyed) return;
	bIsBeingDestroyed = true;

	if (bHasBegunPlay) { EndPlay(); }
	if (bHasBeenInitialized) { Uninitialize(); }
	if (bIsRegistered) { UnregisterFeature(); }

	if (UVSObjectFeature* OwnerFeature = GetOwnerFeature()) { OwnerFeature->RemoveSubFeature(this); }

	MarkAsGarbage();
}


void UVSObjectFeature::SetIsReplicated(bool bShouldReplicate)
{
	if (bReplicates == bShouldReplicate) return;
	bReplicates = bShouldReplicate;
	if (LIKELY(HasAnyFlags(RF_NeedInitialization)))
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(UVSObjectFeature, bReplicates, this);
	}
	else if (AActor* Actor = GetOwnerActor())
	{
		if (Actor->HasAuthority())
		{
			bReplicates ? BeginReplication() : EndReplication();
		}
	}
}

UVSObjectFeature* UVSObjectFeature::AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName)
{
	if (!Class) return nullptr;
	UVSObjectFeature* ObjectFeature = NewObject<UVSObjectFeature>(this, Class, NAME_None);
	if (!ObjectFeature) return nullptr;
	ObjectFeature->FeatureName = OptionalFeatureName;
	AddSubFeatureInternal(ObjectFeature);
	return ObjectFeature;
}

void UVSObjectFeature::RemoveSubFeature(UVSObjectFeature* InFeature, bool bRecursive)
{
	if (!InFeature || !InFeature->GetOwnerFeature()) return;
	if (InFeature->GetOwnerFeature() == this)
	{
		InFeature->DestroyFeature();
		SubFeatures.Remove(InFeature);
	}
	else if (bRecursive && InFeature->GetOwnerFeature()->IsInOuter(this))
	{
		InFeature->GetOwnerFeature()->RemoveSubFeature(InFeature);
	}
}

TArray<UVSObjectFeature*> UVSObjectFeature::GetSubFeatures(bool bRecursive) const
{
	TArray<UVSObjectFeature*> FoundSubFeatures;
	for (UVSObjectFeature* ObjectFeature : SubFeatures)
	{
		if (!ObjectFeature) continue;
		FoundSubFeatures.Add(ObjectFeature);
		if (bRecursive)
		{
			FoundSubFeatures.Append(ObjectFeature->GetSubFeatures(bRecursive));
		}
	}

	return FoundSubFeatures;
}

UVSObjectFeature* UVSObjectFeature::GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive) const
{
	for (UVSObjectFeature* SubFeature : GetSubFeatures(bRecursive))
	{
		if (SubFeature->IsA(Class))
		{
			return SubFeature;
		}
	}
	return nullptr;
}

TArray<UVSObjectFeature*> UVSObjectFeature::GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive) const
{
	TArray<UVSObjectFeature*> OutSubFeatures;
	for (UVSObjectFeature* SubFeature : GetSubFeatures(bRecursive))
	{
		if (SubFeature->IsA(Class))
		{
			OutSubFeatures.Add(SubFeature);
		}
	}
	return OutSubFeatures;
}

UVSObjectFeature* UVSObjectFeature::GetSubFeatureByName(FName Name, bool bRecursive) const
{
	if (Name.IsNone()) return nullptr;
	for (UVSObjectFeature* SubFeature : GetSubFeatures(bRecursive))
	{
		if (SubFeature->FeatureName == Name)
		{
			return SubFeature;
		}
	}
	return nullptr;
}

void UVSObjectFeature::SetActive(bool bNewActive)
{
	if (bIsActive == bNewActive) return;
	bIsActive = bNewActive;
	bIsActive ? OnFeatureActivated() : OnFeatureInactivated();
}

void UVSObjectFeature::OnFeatureActivated_Implementation()
{
	
}

void UVSObjectFeature::OnFeatureInactivated_Implementation()
{
	
}

void UVSObjectFeature::BeginReplication()
{
	if (GetOwnerComponent() && GetOwnerComponent()->GetIsReplicated())
	{
		GetOwnerComponent()->AddReplicatedSubObject(this);
	}
	else if (AActor* Actor = GetOwnerActor())
	{
		Actor->AddReplicatedSubObject(this);
	}
}

void UVSObjectFeature::EndReplication()
{
	if (GetOwnerComponent() && GetOwnerComponent()->IsReplicatedSubObjectRegistered(this))
	{
		GetOwnerComponent()->RemoveReplicatedSubObject(this);
	}
	else if (AActor* Actor = GetOwnerActor())
	{
		Actor->RemoveReplicatedSubObject(this);
	}
}

bool UVSObjectFeature::AddSubFeatureInternal(UVSObjectFeature* InFeature)
{
	if (!InFeature || InFeature->GetOwnerFeature() != this) return false;
	SubFeatures.AddUnique(InFeature);

	if (IsRegistered() && !InFeature->IsRegistered()) { InFeature->RegisterFeature(); }
	
	return true;
}

void UVSObjectFeature::OnRep_bIsActive()
{
	bIsActive ? OnFeatureActivated() : OnFeatureInactivated();
}
