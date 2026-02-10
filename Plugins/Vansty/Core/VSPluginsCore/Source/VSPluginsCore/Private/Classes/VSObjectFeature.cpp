// Copyright VanstySanic. All Rights Reserved.

#include "Classes/VSObjectFeature.h"
#include "Classes/VSTickableObject.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

DEFINE_LOG_CATEGORY(LogObjectFeature);

UVSObjectFeature::UVSObjectFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAutoActivate(true)
	, bRegisterWithOwner(true)
{
	TickProxy = CreateDefaultSubobject<UVSObjectTickProxy>(TEXT("TickProxy"));
}

void UVSObjectFeature::BeginDestroy()
{
	DestroyFeature();
	
	Super::BeginDestroy();
}

FTickFunction* UVSObjectFeature::GetPrimaryTickFunctionPtr() const
{
	return TickProxy ? &TickProxy->PrimaryObjectTick : nullptr;
}

void UVSObjectFeature::PostInitProperties()
{
	Super::PostInitProperties();

	OwnerActorPrivate = GetTypedOuter<AActor>();
	OwnerFeaturePrivate = GetTypedOuter<UVSObjectFeature>();
}

void UVSObjectFeature::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != nullptr) { BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps); }

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UVSObjectFeature, bIsActive, SharedParams);
	DOREPLIFETIME(UVSObjectFeature, SubFeatures);
	DOREPLIFETIME(UVSObjectFeature, OwnerFeaturePrivate);
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
	
	const bool bOwnerActorBeginPlayStarted = OwnerActorPrivate.IsValid() ? (OwnerActorPrivate->HasActorBegunPlay() || OwnerActorPrivate->IsActorBeginningPlay()) : true;
	const bool bOwnerFeatureBeginPlayStarted = OwnerFeaturePrivate ? (OwnerFeaturePrivate->HasBeenInitialized() || OwnerFeaturePrivate->HasBegunPlay()) : true;

	if (!bOwnerActorBeginPlayStarted || !bOwnerFeatureBeginPlayStarted)
	{
		UE_LOG(LogObjectFeature, Log, TEXT("RegisterFeature: (%s) Invalid register time. Aborting."), *GetPathName());
		return;
	}
	
	bIsRegistered = true;

	/**
	 * If not in a game world register ticks now, otherwise defer until BeginPlay.
	 * If no owner we won't trigger BeginPlay either so register now in that case as well.
	 */
	{
		if (OwnerActorPrivate.IsValid() && OwnerActorPrivate->IsActorInitialized() && OwnerActorPrivate->GetIsReplicated()
			&& GetIsReplicated() && IsSupportedForNetworking())
		{
			BeginReplication();
		}

		if (bOwnerActorBeginPlayStarted && bOwnerFeatureBeginPlayStarted)
		{
			if (!HasBeenInitialized())
			{
				Initialize();
				bHasBeenInitialized = true;
			}
			if (!HasBegunPlay())
			{
				BeginPlay();
				bHasBegunPlay = true;
				SetActive(bAutoActivate);
			}

			if (TickProxy)
			{
				SetTickEnabled(true);
			}
		}
	}

	/** Register sub features. */
	TArray<UVSObjectFeature*> CurrentSubFeatures = SubFeatures;
	for (UVSObjectFeature* SubFeature : CurrentSubFeatures)
	{
		if (SubFeature && SubFeature->bRegisterWithOwner)
		{
			if (SubFeature->IsRegistered())
			{
				SubFeature->UnregisterFeature();
			}
			SubFeature->RegisterFeature();
		}
	}
}

void UVSObjectFeature::UnregisterFeature()
{
	/** Unregister sub features. */
	TArray<UVSObjectFeature*> CopiedSubFeatures = SubFeatures;
	for (UVSObjectFeature* SubFeature : CopiedSubFeatures)
	{
		if (SubFeature && SubFeature->IsRegistered())
		{
			SubFeature->UnregisterFeature();
		}
	}

	if(!IsValid(this))
	{
		if (IsValid(GetWorld()) && !GetWorld()->bIsTearingDown)
		{
			UE_LOG(LogObjectFeature, Log, TEXT("UnregisterFeature: (%s) Trying to unregister feature with IsValid() == false. Aborting."), *GetPathName());
		}

		return;
	}
	
	if(!IsRegistered())
	{
		UE_LOG(LogObjectFeature, Log, TEXT("UnregisterFeature: (%s) Not registered. Aborting."), *GetPathName());
		return;
	}
	
	bIsRegistered = false;

	if (bIsActive) { SetActive(false); }
	if (bHasBegunPlay)
	{
		EndPlay();
		bHasBegunPlay = false;
	}
	if (bHasBeenInitialized)
	{
		Uninitialize();
		bHasBeenInitialized = false;
	}
	if (GetIsReplicated()) { EndReplication(); }

	if (TickProxy)
	{
		SetTickEnabled(false);
	}
}

void UVSObjectFeature::DestroyFeature()
{
	if (bIsBeingDestroyed || !IsValid(this)) return;
	bIsBeingDestroyed = true;

	/** Destroy sub features. */
	TArray<UVSObjectFeature*> SubFeaturesToDestroy = SubFeatures;
	for (UVSObjectFeature* SubFeature : SubFeaturesToDestroy)
	{
		if (SubFeature)
		{
			SubFeature->DestroyFeature();
		}
	}
	
	if (bIsRegistered) { UnregisterFeature(); }

	if (OwnerFeaturePrivate)
	{
		OwnerFeaturePrivate->SubFeatures.Remove(this);
	}
	
	if (IsRooted())
	{
		RemoveFromRoot();
	}
	MarkAsGarbage();
}

void UVSObjectFeature::SetActive(bool bNewActive)
{
	if (bIsActive == bNewActive) return;
	bIsActive = bNewActive;
	MARK_PROPERTY_DIRTY_FROM_NAME(UVSObjectFeature, bIsActive, this);
	bIsActive ? OnFeatureActivated() : OnFeatureDeactivated();
}

TArray<UVSObjectFeature*> UVSObjectFeature::GetSubFeatures() const
{
	TArray<UVSObjectFeature*> Features;
	for (UVSObjectFeature* SubFeature : SubFeatures)
	{
		if (SubFeature)
		{
			Features.Add(SubFeature);
			Features.Append(SubFeature->GetSubFeatures());
		}
	}
	return Features;
}

bool UVSObjectFeature::HasSubFeature(UVSObjectFeature* Feature) const
{
	if (SubFeatures.Contains(Feature)) return true;
	for (UVSObjectFeature* SubFeature : SubFeatures)
	{
		if (SubFeature && SubFeature->HasSubFeature(Feature)) return true;
	}

	return false;
}

void UVSObjectFeature::AddSubFeatureInstance(UVSObjectFeature* Feature, bool bDeferRegister)
{
	if (!Feature || Feature->OwnerFeaturePrivate == this) return;
	
	if (Feature == this)
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("AddInstancedFeature: (%s) trying to set it self as sub feature. Aborting."), *GetPathName());
		return;
	}

	if (HasOwnerFeature(Feature) || IsInOuter(Feature))
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("AddInstancedFeature: (%s) trying to set it's owner feature as child. Aborting."), *GetPathName());
		return;
	}
	
	if (Feature->OwnerActorPrivate.IsValid() && Feature->OwnerActorPrivate != OwnerActorPrivate)
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("AddInstancedFeature: (%s) has different owner actor from (%s). Aborting."), *GetPathName(), *Feature->GetPathName());
		return;
	}

	if (Feature->OwnerFeaturePrivate)
	{
		Feature->OwnerFeaturePrivate->RemoveSubFeature(Feature);
	}
	/** Force re-register sub feature when reparenting. */
	else if (Feature->IsRegistered())
	{
		Feature->UnregisterFeature();
	}
	
	SubFeatures.Add(Feature);
	Feature->OwnerFeaturePrivate = this;

	if (Feature->IsRegistered() && !IsRegistered())
	{
		Feature->UnregisterFeature();
	}
	if (!bDeferRegister && Feature->bRegisterWithOwner &&  IsRegistered())
	{
		Feature->RegisterFeature();
	}
}

UVSObjectFeature* UVSObjectFeature::AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bDeferRegister)
{
	if (!Class) return nullptr;
	UVSObjectFeature* Feature = NewObject<UVSObjectFeature>(this, Class);
	
	SubFeatures.Add(Feature);
	Feature->OwnerFeaturePrivate = this;
	
	if (!bDeferRegister && Feature->bRegisterWithOwner && IsRegistered())
	{
		Feature->RegisterFeature();
	}
	
	return Feature;
}

void UVSObjectFeature::RemoveSubFeature(UVSObjectFeature* Feature)
{
	if (!Feature || Feature == this || !HasSubFeature(Feature)) return;

	if (Feature->IsRegistered())
	{
		Feature->UnregisterFeature();
	}

	if (Feature->OwnerFeaturePrivate)
	{
		Feature->OwnerFeaturePrivate->SubFeatures.RemoveSingle(Feature);
		Feature->OwnerFeaturePrivate = nullptr;
	}
}

void UVSObjectFeature::SetOwnerFeature(UVSObjectFeature* Feature, bool bDeferRegister)
{
	if (Feature == OwnerFeaturePrivate) return;
	
	if (Feature == this)
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("SetOwnerFeature: (%s) trying to set it self as owner. Aborting."), *GetPathName());
		return;
	}

	if (HasSubFeature(Feature) || Feature->IsInOuter(this))
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("SetOwnerFeature: (%s) trying to set it's sub feature as owner. Aborting."), *GetPathName());
		return;
	}
	
	if (Feature->OwnerActorPrivate.IsValid() && Feature->OwnerActorPrivate != OwnerActorPrivate)
	{
		UE_LOG(LogObjectFeature, Warning, TEXT("SetOwnerFeature: (%s) has different owner actor from (%s). Aborting."), *GetPathName(), *Feature->GetPathName());
		return;
	}

	if (OwnerFeaturePrivate)
	{
		OwnerFeaturePrivate->RemoveSubFeature(this);
	}
	/** Force re-register feature when reparenting. */
	else if (IsRegistered())
	{
		UnregisterFeature();
	}

	if (Feature)
	{
		Feature->SubFeatures.Add(this);
	}
	
	OwnerFeaturePrivate = Feature;
	
	if (!bDeferRegister && Feature && Feature->IsRegistered())
	{
		RegisterFeature();
	}
}

bool UVSObjectFeature::HasOwnerFeature(UVSObjectFeature* Feature) const
{
	if (!Feature) return false;
	if (OwnerFeaturePrivate == Feature) return true;
	if (OwnerFeaturePrivate)
	{
		return OwnerFeaturePrivate->HasOwnerFeature(Feature);
	}
	return false;
}

UVSObjectFeature* UVSObjectFeature::GetOwnerFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const
{
	if (!OwnerFeaturePrivate) return nullptr;
	if (OwnerFeaturePrivate->IsA(Class))
	{
		return OwnerFeaturePrivate.Get();
	}

	return OwnerFeaturePrivate->GetOwnerFeatureByClass(Class);
}

UVSObjectFeature* UVSObjectFeature::GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const
{
	for (UVSObjectFeature* SubFeature : SubFeatures)
	{
		if (SubFeature && SubFeature->IsA(Class))
		{
			return SubFeature;
		}
	}

	for (UVSObjectFeature* SubFeature : SubFeatures)
	{
		if (SubFeature)
		{
			if (UVSObjectFeature* Feature = SubFeature->GetSubFeatureByClass(Class))
			{
				return Feature;
			}
		}
	}
	
	return nullptr;
}

TArray<UVSObjectFeature*> UVSObjectFeature::GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class) const
{
	TArray<UVSObjectFeature*> OutSubFeatures;
	for (UVSObjectFeature* SubFeature : GetSubFeatures())
	{
		if (SubFeature->IsA(Class))
		{
			OutSubFeatures.Add(SubFeature);
		}
	}
	return OutSubFeatures;
}

UVSObjectFeature* UVSObjectFeature::GetSubFeatureByName(FName Name) const
{
	if (Name.IsNone()) return nullptr;
	for (UVSObjectFeature* SubFeature : SubFeatures)
	{
		if (SubFeature && SubFeature->FeatureName == Name)
		{
			return SubFeature;
		}
	}

	for (UVSObjectFeature* SubFeature : SubFeatures)
	{
		if (SubFeature)
		{
			if (UVSObjectFeature* Feature = SubFeature->GetSubFeatureByName(Name))
			{
				return Feature;
			}
		}
	}
	
	return nullptr;
}

void UVSObjectFeature::Initialize_Implementation()
{
}

void UVSObjectFeature::Uninitialize_Implementation()
{
}

void UVSObjectFeature::BeginPlay_Implementation()
{

}

void UVSObjectFeature::EndPlay_Implementation()
{
}

bool UVSObjectFeature::CanTick_Implementation() const
{
	return TickProxy && TickProxy->PrimaryObjectTick.bCanEverTick && TickProxy->IsTickFunctionRegistered() && TickProxy->IsTickFunctionEnabled()
		&& IsActive();
}

void UVSObjectFeature::OnCreatedFromReplication()
{
	Super::OnCreatedFromReplication();
}

void UVSObjectFeature::OnDestroyedFromReplication()
{
	DestroyFeature();
	Super::OnDestroyedFromReplication();
}

AActor* UVSObjectFeature::GetOwnerActor() const
{
	return OwnerActorPrivate.IsValid() ? OwnerActorPrivate.Get() : GetTypedOuter<AActor>();
}

void UVSObjectFeature::SetTickEnabled(bool bEnabled)
{
	if (!TickProxy) return;
	if (bEnabled && !TickProxy->IsTickFunctionRegistered())
	{
		TickProxy->OnTick_Native.AddUObject(this, &UVSObjectFeature::ReceiveProxyTick);
		TickProxy->CanTick_Native.BindLambda([this] (UVSObjectTickProxy* PrimaryTickProxy)
		{
			return CanTick();
		});
		TickProxy->RegisterTickFunction();
	}
	else if (!bEnabled && TickProxy->IsTickFunctionRegistered())
	{
		TickProxy->OnTick_Native.RemoveAll(this);
		TickProxy->CanTick_Native.Unbind();
		TickProxy->UnregisterTickFunction();
	}
}

void UVSObjectFeature::TickFeature(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction)
{
	Tick(DeltaTime);
}

void UVSObjectFeature::Tick_Implementation(float DeltaTime)
{
	
}

void UVSObjectFeature::OnFeatureActivated_Implementation()
{

}

void UVSObjectFeature::OnFeatureDeactivated_Implementation()
{

}

void UVSObjectFeature::ReceiveProxyTick(UVSObjectTickProxy* PrimaryTickProxy, float DeltaTime, ELevelTick TickType,FVSObjectTickFunction* TickFunction)
{
	TickFeature(DeltaTime, TickType, TickFunction);
}

void UVSObjectFeature::OnRep_bIsActive()
{
	bIsActive ? OnFeatureActivated() : OnFeatureDeactivated();
}

void UVSObjectFeature::OnRep_OwnerFeaturePrivate(UVSObjectFeature* PrevOwnerFeature)
{
	if (OwnerFeaturePrivate == PrevOwnerFeature) return;
	if (PrevOwnerFeature)
	{
		if (PrevOwnerFeature->HasSubFeature(this))
		{
			PrevOwnerFeature->RemoveSubFeature(this);
		}
		else if (IsRegistered())
		{
			UnregisterFeature();
		}
	}
	if (OwnerFeaturePrivate)
	{
		if (!OwnerFeaturePrivate->HasSubFeature(this))
		{
			OwnerFeaturePrivate->AddSubFeatureInstance(this);
		}
		else if (!IsRegistered())
		{
			RegisterFeature();
		}
	}
}