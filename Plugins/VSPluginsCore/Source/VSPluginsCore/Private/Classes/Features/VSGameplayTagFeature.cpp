// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSGameplayTagFeature.h"
#include "VSPluginsCoreCpp/Public/VSPrivablic.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Classes/VSPluginsCoreSettings.h"
#include "Classes/Libraries/VSActorLibrary.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED, "VS.PluginsCore.GameplayTagFeature.OnTagsUpdated")

VS_DECLARE_PRIVABLIC_MEMBER(UAbilitySystemComponent, GameplayTagCountContainer, FGameplayTagCountContainer);

UVSGameplayTagFeature::UVSGameplayTagFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
}

void UVSGameplayTagFeature::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != nullptr) { BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps); }

	DOREPLIFETIME_CONDITION(UVSGameplayTagFeature, InitialAutonomousReplicatedTagCounts, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UVSGameplayTagFeature, InitialSimulationReplicatedTagCounts, COND_InitialOnly);
}

void UVSGameplayTagFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (bUseAbilitySystemComponentSource)
	{
		AbilitySystemComponentPrivate = UVSActorLibrary::GetAbilitySystemComponentFormActor(GetOwnerActor());
		check(AbilitySystemComponentPrivate.IsValid());
		GameplayTagCountContainerPtr = &VS_PRIVABLIC_MEMBER(AbilitySystemComponentPrivate.Get(), UAbilitySystemComponent, GameplayTagCountContainer);
		check(GameplayTagCountContainerPtr);

		for (const FGameplayTag& GameplayTag : UVSPluginsCoreSettings::Get()->AbilitySystemComponentListeningTags.GetGameplayTagArray())
		{
			FDelegateHandle DelegateHandle = AbilitySystemComponentPrivate->RegisterGameplayTagEvent(GameplayTag, EGameplayTagEventType::AnyCountChange)
				.AddUObject(this, &UVSGameplayTagFeature::OnAbilitySystemComponentListeningTagsUpdated);
			AbilitySystemComponentRegisteredDelegateHandles.Emplace(GameplayTag, DelegateHandle);
		}
	}
	
	/** Auto bind delegates. */
	if (bBindDelegatesWhenInitialized)
	{
		BindDelegateForObject(GetOwnerActor());
	}
}

void UVSGameplayTagFeature::Uninitialize_Implementation()
{
	if (bUseAbilitySystemComponentSource)
	{
		if (AbilitySystemComponentPrivate.IsValid())
		{
			for (const TPair<FGameplayTag, FDelegateHandle>& AbilitySystemComponentRegisteredDelegateHandle : AbilitySystemComponentRegisteredDelegateHandles)
			{
				if (AbilitySystemComponentRegisteredDelegateHandle.Value.IsValid())
				{
					AbilitySystemComponentPrivate->UnregisterGameplayTagEvent(AbilitySystemComponentRegisteredDelegateHandle.Value, AbilitySystemComponentRegisteredDelegateHandle.Key, EGameplayTagEventType::AnyCountChange);
				}
			}
		}
		GameplayTagCountContainerPtr = nullptr;
		AbilitySystemComponentPrivate.Reset();
		AbilitySystemComponentRegisteredDelegateHandles.Empty();
	}
	
	Super::Uninitialize_Implementation();
}

void UVSGameplayTagFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	/** Refresh from replication for safety. */
	OnRep_InitialAutonomousReplicatedTagCounts();
	OnRep_InitialSimulationReplicatedTagCounts();
}

void UVSGameplayTagFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (bNotifyTagsUpdateDuringTicks)
	{
		NotifyTagsUpdated();
	}

#if WITH_EDITORONLY_DATA
	if (bPrintDebugString)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, GetDebugString(), false);
	}
#endif
}

void UVSGameplayTagFeature::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (bUseAbilitySystemComponentSource)
	{
		if (AbilitySystemComponentPrivate.IsValid())
		{
			AbilitySystemComponentPrivate->GetOwnedGameplayTags(TagContainer);
		}
		else
		{
			TagContainer.Reset();
		}
	}
	else
	{
		TagContainer = LocalGameplayTags;
	}
}

bool UVSGameplayTagFeature::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->HasMatchingGameplayTag(TagToCheck) : false;
	}

	return LocalGameplayTags.HasTag(TagToCheck);
}

bool UVSGameplayTagFeature::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->HasAllMatchingGameplayTags(TagContainer) : false;
	}

	return LocalGameplayTags.HasAll(TagContainer);
}

bool UVSGameplayTagFeature::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->HasAnyMatchingGameplayTags(TagContainer) : false;
	}

	return LocalGameplayTags.HasAny(TagContainer);
}

FGameplayTagContainer UVSGameplayTagFeature::GetGameplayTags() const
{
	if (bUseAbilitySystemComponentSource)
	{
		if (AbilitySystemComponentPrivate.IsValid())
		{
			return AbilitySystemComponentPrivate->GetOwnedGameplayTags();
		}
	}

	return LocalGameplayTags;
}

bool UVSGameplayTagFeature::HasTag(const FGameplayTag& TagToCheck, bool bExact) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return (AbilitySystemComponentPrivate.IsValid() && GameplayTagCountContainerPtr)
	       ? (bExact ? GameplayTagCountContainerPtr->GetExplicitTagCount(TagToCheck) > 0 : GameplayTagCountContainerPtr->GetTagCount(TagToCheck) > 0)
	       : false;
	}

	return bExact ? LocalGameplayTags.HasTagExact(TagToCheck) : LocalGameplayTags.HasTag(TagToCheck);
}

bool UVSGameplayTagFeature::HasAnyTag(const FGameplayTagContainer& TagsToCheck, bool bExact) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return (AbilitySystemComponentPrivate.IsValid() && GameplayTagCountContainerPtr)
			? (bExact ? GameplayTagCountContainerPtr->GetExplicitGameplayTags().HasAnyExact(TagsToCheck) : GameplayTagCountContainerPtr->GetExplicitGameplayTags().HasAny(TagsToCheck))
			: false;
	}

	return bExact ? LocalGameplayTags.HasAnyExact(TagsToCheck) : LocalGameplayTags.HasAny(TagsToCheck);
}

bool UVSGameplayTagFeature::HasAllTags(const FGameplayTagContainer& TagsToCheck, bool bExact) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return (AbilitySystemComponentPrivate.IsValid() && GameplayTagCountContainerPtr)
			? (bExact ? GameplayTagCountContainerPtr->GetExplicitGameplayTags().HasAllExact(TagsToCheck) : GameplayTagCountContainerPtr->GetExplicitGameplayTags().HasAll(TagsToCheck))
			: false;
	}
	
	return bExact ? LocalGameplayTags.HasAllExact(TagsToCheck) : LocalGameplayTags.HasAll(TagsToCheck);
}

int32 UVSGameplayTagFeature::GetTagCount(const FGameplayTag& TagToCheck, bool bExact) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return (AbilitySystemComponentPrivate.IsValid() && GameplayTagCountContainerPtr)
	       ? (bExact ? GameplayTagCountContainerPtr->GetExplicitTagCount(TagToCheck) : GameplayTagCountContainerPtr->GetTagCount(TagToCheck))
	       : 0;
	}

	return bExact ? LocalExplicitTagCounts.FindRef(TagToCheck) : LocalImplicitTagCounts.FindRef(TagToCheck);
}

bool UVSGameplayTagFeature::MatchesTagQuery(const FGameplayTagQuery& TagQuery, bool bEmptyQueryPass) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return (AbilitySystemComponentPrivate.IsValid() && GameplayTagCountContainerPtr)
			? ((bEmptyQueryPass && TagQuery.IsEmpty()) || TagQuery.Matches(GameplayTagCountContainerPtr->GetExplicitGameplayTags()))
			: false;
	}

	return TagQuery.Matches(LocalGameplayTags);
}

bool UVSGameplayTagFeature::MatchesEventQuery(const FVSGameplayTagEventQuery& EventQuery, const FGameplayTagContainer& TagEvents) const
{
	if (bUseAbilitySystemComponentSource)
	{
		return (AbilitySystemComponentPrivate.IsValid())
	       ? EventQuery.Matches(TagEvents, AbilitySystemComponentPrivate->GetOwnedGameplayTags())
	       : false;
	}

	return EventQuery.Matches(TagEvents, LocalGameplayTags);
}

void UVSGameplayTagFeature::AddTag(const FGameplayTag& GameplayTag, int32 AddCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GameplayTag == FGameplayTag::EmptyTag || AddCount <= 0) return;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			DeltaTagCountInternal(GameplayTag, AddCount);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			DeltaTagCount_Server(GameplayTag, AddCount, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		DeltaTagCount_Server(GameplayTag, AddCount, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			DeltaTagCountInternal(GameplayTag, AddCount);
		}
	}
}

void UVSGameplayTagFeature::AddTags(const FGameplayTagContainer& GameplayTags, int32 AddCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GameplayTags.IsEmpty() || AddCount <= 0) return;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			DeltaTagsCountInternal(GameplayTags, AddCount);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			DeltaTagsCount_Server(GameplayTags, AddCount, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		DeltaTagsCount_Server(GameplayTags, AddCount, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			DeltaTagsCountInternal(GameplayTags, AddCount);
		}
	}
}

void UVSGameplayTagFeature::RemoveTag(const FGameplayTag& GameplayTag, int32 RemoveCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GameplayTag == FGameplayTag::EmptyTag || RemoveCount <= 0) return;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			DeltaTagCountInternal(GameplayTag, -RemoveCount);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			DeltaTagCount_Server(GameplayTag, -RemoveCount, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		DeltaTagCount_Server(GameplayTag, -RemoveCount, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			DeltaTagCountInternal(GameplayTag, -RemoveCount);
		}
	}
}

void UVSGameplayTagFeature::RemoveTags(const FGameplayTagContainer& GameplayTags, int32 RemoveCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GameplayTags.IsEmpty() || RemoveCount <= 0) return;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			DeltaTagsCountInternal(GameplayTags, -RemoveCount);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			DeltaTagsCount_Server(GameplayTags, -RemoveCount, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		DeltaTagsCount_Server(GameplayTags, -RemoveCount, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			DeltaTagsCountInternal(GameplayTags, -RemoveCount);
		}
	}
}

void UVSGameplayTagFeature::SetTagCount(const FGameplayTag& GameplayTag, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GameplayTag == FGameplayTag::EmptyTag) return;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			SetTagCountInternal(GameplayTag, NewCount);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			SetTagCount_Server(GameplayTag, NewCount, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		SetTagCount_Server(GameplayTag, NewCount, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			SetTagCountInternal(GameplayTag, NewCount);
		}
	}
}

void UVSGameplayTagFeature::SetTagsCount(const FGameplayTagContainer& GameplayTags, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GameplayTags.IsEmpty()) return;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			SetTagsCountInternal(GameplayTags, NewCount);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			SetTagsCount_Server(GameplayTags, NewCount, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		SetTagsCount_Server(GameplayTags, NewCount, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			SetTagsCountInternal(GameplayTags, NewCount);
		}
	}
}

void UVSGameplayTagFeature::NotifyTagsUpdated(bool bAllowCleanNotify, const FGameplayTagContainer& OptionalTagEvents)
{
	if (!bAllowCleanNotify && !IsDirty()) return;
	bTagsDirty = false;
	OnTagsUpdated_Native.Broadcast(this);
	OnTagsUpdated.Broadcast(this);
	
	FGameplayTagContainer TagEvents = OptionalTagEvents;
	TagEvents.AddTag(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED);
	NotifyTagEvents(TagEvents);
}

void UVSGameplayTagFeature::NotifyTagEvent(const FGameplayTag& TagEvent, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			NotifyTagEventInternal(TagEvent);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			NotifyTagEvent_Server(TagEvent, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		NotifyTagEvent_Server(TagEvent, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			NotifyTagEventInternal(TagEvent);
		}
	}
}

void UVSGameplayTagFeature::NotifyTagEvents(const FGameplayTagContainer& TagEvents, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			NotifyTagEventsInternal(TagEvents);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			NotifyTagEvents_Server(TagEvents, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		NotifyTagEvents_Server(TagEvents, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			NotifyTagEventsInternal(TagEvents);
		}
	}
}

void UVSGameplayTagFeature::BindDelegateForObject(UObject* Object)
{
	if (!Object || !Object->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass())) return;

	/** Unbind first for safety. */
	UnbindDelegateForObject(Object);
	
	TScriptDelegate Delegate;
	Delegate.BindUFunction(Object, "OnGameplayTagFeatureTagsUpdated");
	OnTagsUpdated.Add(Delegate);

	Delegate.Unbind();
	Delegate.BindUFunction(Object, "OnGameplayTagFeatureTagEventsNotified");
	OnTagEventsNotified.Add(Delegate);

	/** Immediately push current state once so the listener starts from a consistent view. */
	IVSGameplayTagFeatureInterface::Execute_OnGameplayTagFeatureTagsUpdated(Object, this);
	IVSGameplayTagFeatureInterface::Execute_OnGameplayTagFeatureTagEventsNotified(Object, this, FGameplayTagContainer(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED));
}

void UVSGameplayTagFeature::UnbindDelegateForObject(UObject* Object)
{
	if (!Object || !Object->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass())) return;
	
	TScriptDelegate Delegate;
	Delegate.BindUFunction(Object, "OnGameplayTagFeatureTagsUpdated");
	OnTagsUpdated.Remove(Delegate);
	
	Delegate.Unbind();
	Delegate.BindUFunction(Object, "OnGameplayTagFeatureTagEventsNotified");
	OnTagEventsNotified.Remove(Delegate);
}

FString UVSGameplayTagFeature::GetDebugString()
{
#if WITH_EDITORONLY_DATA
	FString String = "Tags in Feature: " + GetPathName() + " ("
		+ UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()) + " / "
		+ UEnum::GetValueAsString(GetOwnerActor()->GetRemoteRole()) + ") :";
	
	const FGameplayTagContainer& GameplayTags = GetGameplayTags();
	if (GameplayTags.Num())
	{
		for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray())
		{
			String += "\n    " + GameplayTag.ToString() + " (" + FString::FromInt(GetTagCount(GameplayTag)) + ")";
		}
	}
	else
	{
		String += "\n    " + FString("None");
	}
	
	return String;
#else
	return FString();
#endif
}

void UVSGameplayTagFeature::DeltaTagCountInternal(const FGameplayTag& GameplayTag, int32 DeltaCount)
{
	const int32 PrevCount = GetTagCount(GameplayTag);
	const int32 DesiredCount = FMath::Max(0, PrevCount + DeltaCount);
	if (DesiredCount == PrevCount) return;
	
	bool bCachedNotifyInstantly = bNotifyTagsUpdateInstantly;
	bNotifyTagsUpdateInstantly = false;
	SetTagCountInternal(GameplayTag, DesiredCount);
	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
	
	const int32 CurrentCount = GetTagCount(GameplayTag);
	if (CurrentCount == PrevCount) return;
	
	if (bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}

void UVSGameplayTagFeature::DeltaTagsCountInternal(const FGameplayTagContainer& GameplayTags, int32 DeltaCount)
{
	if (DeltaCount == 0) return;

	bool bCachedNotifyInstantly = bNotifyTagsUpdateInstantly;
	bNotifyTagsUpdateInstantly = false;
	
	bool bHasChanges = false;
	TArray<FGameplayTag> Tags;
	GameplayTags.GetGameplayTagArray(Tags);
	for (const FGameplayTag& Tag : Tags)
	{
		const int32 PrevCount = GetTagCount(Tag);
		DeltaTagCountInternal(Tag, DeltaCount);
		const int32 CurrentCount = GetTagCount(Tag);

		if (!bHasChanges && CurrentCount != PrevCount)
		{
			bHasChanges = true;
		}
	}

	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
	
	if (bHasChanges && bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}

void UVSGameplayTagFeature::SetTagCountInternal(const FGameplayTag& GameplayTag, int32 Count)
{
	const int32 PrevCount = GetTagCount(GameplayTag);
	const int32 DesiredCount = FMath::Max(0, Count);
	if (DesiredCount == PrevCount) return;
	
	if (bUseAbilitySystemComponentSource)
	{
		if (AbilitySystemComponentPrivate.IsValid())
		{
			AbilitySystemComponentPrivate->SetTagMapCount(GameplayTag, DesiredCount);
		}
	}
	else
	{
		if (PrevCount == 0 && DesiredCount > 0)
		{
			LocalGameplayTags.AddTagFast(GameplayTag);
			LocalExplicitTagCounts.FindOrAdd(GameplayTag) = DesiredCount;
		}
		else if (PrevCount > 0 && DesiredCount == 0)
		{
			LocalGameplayTags.RemoveTag(GameplayTag);
			LocalExplicitTagCounts.Remove(GameplayTag);
		}
		else if (DesiredCount > 0)
		{
			LocalExplicitTagCounts.FindOrAdd(GameplayTag) = DesiredCount;
		}
	}
	const int32 CurrentCount = GetTagCount(GameplayTag);
	if (CurrentCount == PrevCount) return;

	if (!bUseAbilitySystemComponentSource)
	{
		/** Update implicit tag count. */
		for (const FGameplayTag& Tag : GameplayTag.GetGameplayTagParents())
		{
			const int32 PrevImplicitCount = LocalImplicitTagCounts.FindRef(Tag);
			const int32 TargetImplicitCount = PrevImplicitCount + (CurrentCount - PrevCount);
			if (TargetImplicitCount > 0)
			{
				LocalImplicitTagCounts.FindOrAdd(Tag) = TargetImplicitCount;
			}
			else if (PrevImplicitCount > 0 && TargetImplicitCount == 0)
			{
				LocalImplicitTagCounts.Remove(Tag);
			}
		}
	}
		
	bTagsDirty = true;
		
	UpdateInitialReplicatedTag(GameplayTag);
	if (bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}

void UVSGameplayTagFeature::SetTagsCountInternal(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	bool bCachedNotifyInstantly = bNotifyTagsUpdateInstantly;
	bNotifyTagsUpdateInstantly = false;
	
	bool bHasChanges = false;
	const FGameplayTagContainer& Tags = GameplayTags;
	for (const FGameplayTag& Tag : Tags)
	{
		const int32 PrevCount = GetTagCount(Tag);
		SetTagCountInternal(Tag, Count);
		const int32 CurrentCount = GetTagCount(Tag);

		if (!bHasChanges && CurrentCount != PrevCount)
		{
			bHasChanges = true;
		}
	}
	
	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;

	if (bHasChanges && bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}

void UVSGameplayTagFeature::UpdateInitialReplicatedTag(const FGameplayTag& GameplayTag)
{
	if (!GetOwnerActor()->HasAuthority()) return;
	const int32 CurrentCount = GetTagCount(GameplayTag);
	if (UVSPluginsCoreSettings::Get()->InitialAutonomousReplicatedGameplayTags.HasTagExact(GameplayTag))
	{
		const int32 CurrentIndex = InitialAutonomousReplicatedTagCounts.Tags.Find(GameplayTag);
		if (CurrentIndex == INDEX_NONE && CurrentCount > 0)
		{
			InitialAutonomousReplicatedTagCounts.Tags.Add(GameplayTag);
			InitialAutonomousReplicatedTagCounts.Counts.Add(CurrentCount);
		}
		else if (CurrentIndex != INDEX_NONE && CurrentCount == 0)
		{
			InitialAutonomousReplicatedTagCounts.Tags.RemoveAt(CurrentIndex);
			InitialAutonomousReplicatedTagCounts.Counts.RemoveAt(CurrentIndex);
		}
		else if (CurrentIndex != INDEX_NONE && CurrentCount > 0)
		{
			InitialAutonomousReplicatedTagCounts.Counts[CurrentIndex] = CurrentCount;
		}
	}

	if (UVSPluginsCoreSettings::Get()->InitialSimulationReplicatedGameplayTags.HasTagExact(GameplayTag))
	{
		const int32 CurrentIndex = InitialSimulationReplicatedTagCounts.Tags.Find(GameplayTag);
		if (CurrentIndex == INDEX_NONE && CurrentCount > 0)
		{
			InitialSimulationReplicatedTagCounts.Tags.Add(GameplayTag);
			InitialSimulationReplicatedTagCounts.Counts.Add(CurrentCount);
		}
		else if (CurrentIndex != INDEX_NONE && CurrentCount == 0)
		{
			InitialSimulationReplicatedTagCounts.Tags.RemoveAt(CurrentIndex);
			InitialSimulationReplicatedTagCounts.Counts.RemoveAt(CurrentIndex);
		}
		else if (CurrentIndex != INDEX_NONE && CurrentCount > 0)
		{
			InitialSimulationReplicatedTagCounts.Counts[CurrentIndex] = CurrentCount;
		}
	}
}

void UVSGameplayTagFeature::UpdateInitialReplicatedTags(const FGameplayTagContainer& GameplayTags)
{
	for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray())
	{
		UpdateInitialReplicatedTag(GameplayTag);	
	}
}

void UVSGameplayTagFeature::NotifyTagEventInternal(const FGameplayTag& TagEvent)
{
	if (TagEvent == FGameplayTag::EmptyTag) return;
	NotifyTagEventsInternal(FGameplayTagContainer(TagEvent));
}

void UVSGameplayTagFeature::NotifyTagEventsInternal(const FGameplayTagContainer& TagEvents)
{
	OnTagEventsNotified_Native.Broadcast(this,TagEvents);
	OnTagEventsNotified.Broadcast(this, TagEvents);
}

void UVSGameplayTagFeature::DeltaTagCount_Server_Implementation(const FGameplayTag& GameplayTag, int32 DeltaCount, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		DeltaTagCountInternal(GameplayTag, DeltaCount);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)
	{
		DeltaTagCount_Client(GameplayTag, DeltaCount);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)
	{
		DeltaTagCount_MultiCast(GameplayTag, DeltaCount);
	}
}

void UVSGameplayTagFeature::DeltaTagCount_Client_Implementation(const FGameplayTag& GameplayTag, int32 DeltaCount)
{
	DeltaTagCountInternal(GameplayTag, DeltaCount);
}

void UVSGameplayTagFeature::DeltaTagCount_MultiCast_Implementation(const FGameplayTag& GameplayTag, int32 DeltaCount)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	DeltaTagCountInternal(GameplayTag, DeltaCount);
}

void UVSGameplayTagFeature::DeltaTagsCount_Server_Implementation(const FGameplayTagContainer& GameplayTags, int32 DeltaCount, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		DeltaTagsCountInternal(GameplayTags, DeltaCount);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)
	{
		DeltaTagsCount_Client(GameplayTags, DeltaCount);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)
	{
		DeltaTagsCount_MultiCast(GameplayTags, DeltaCount);
	}
}

void UVSGameplayTagFeature::DeltaTagsCount_Client_Implementation(const FGameplayTagContainer& GameplayTags, int32 DeltaCount)
{
	DeltaTagsCountInternal(GameplayTags, DeltaCount);
}

void UVSGameplayTagFeature::DeltaTagsCount_MultiCast_Implementation(const FGameplayTagContainer& GameplayTags, int32 DeltaCount)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	DeltaTagsCountInternal(GameplayTags, DeltaCount);
}

void UVSGameplayTagFeature::SetTagCount_Server_Implementation(const FGameplayTag& GameplayTag, int32 Count, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		SetTagCountInternal(GameplayTag, Count);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)
	{
		SetTagCount_Client(GameplayTag, Count);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)
	{
		SetTagCount_MultiCast(GameplayTag, Count);
	}
}

void UVSGameplayTagFeature::SetTagCount_Client_Implementation(const FGameplayTag& GameplayTag, int32 Count)
{
	SetTagCountInternal(GameplayTag, Count);
}

void UVSGameplayTagFeature::SetTagCount_MultiCast_Implementation(const FGameplayTag& GameplayTag, int32 Count)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	SetTagCountInternal(GameplayTag, Count);
}

void UVSGameplayTagFeature::SetTagsCount_Server_Implementation(const FGameplayTagContainer& GameplayTags, int32 Count, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		SetTagsCountInternal(GameplayTags, Count);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)
	{
		SetTagsCount_Client(GameplayTags, Count);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)
	{
		SetTagsCount_MultiCast(GameplayTags, Count);
	}
}

void UVSGameplayTagFeature::SetTagsCount_Client_Implementation(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	SetTagsCountInternal(GameplayTags, Count);
}

void UVSGameplayTagFeature::SetTagsCount_MultiCast_Implementation(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	SetTagsCountInternal(GameplayTags, Count);
}

void UVSGameplayTagFeature::NotifyTagEvent_Server_Implementation(const FGameplayTag& GameplayTag, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		NotifyTagEventInternal(GameplayTag);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)
	{
		NotifyTagEvent_Client(GameplayTag);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)
	{
		NotifyTagEvent_MultiCast(GameplayTag);
	}
}

void UVSGameplayTagFeature::NotifyTagEvent_Client_Implementation(const FGameplayTag& GameplayTag)
{
	NotifyTagEventInternal(GameplayTag);
}

void UVSGameplayTagFeature::NotifyTagEvent_MultiCast_Implementation(const FGameplayTag& GameplayTag)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	NotifyTagEventInternal(GameplayTag);
}

void UVSGameplayTagFeature::NotifyTagEvents_Server_Implementation(const FGameplayTagContainer& GameplayTags, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		NotifyTagEventsInternal(GameplayTags);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)
	{
		NotifyTagEvents_Client(GameplayTags);
	}
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)
	{
		NotifyTagEvents_MultiCast(GameplayTags);
	}
}

void UVSGameplayTagFeature::NotifyTagEvents_Client_Implementation(const FGameplayTagContainer& GameplayTags)
{
	NotifyTagEventsInternal(GameplayTags);
}

void UVSGameplayTagFeature::NotifyTagEvents_MultiCast_Implementation(const FGameplayTagContainer& GameplayTags)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	NotifyTagEventsInternal(GameplayTags);
}

void UVSGameplayTagFeature::OnRep_InitialAutonomousReplicatedTagCounts()
{
	if (!GetOwnerActor() || GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	const int32 Num = InitialAutonomousReplicatedTagCounts.Tags.Num();
	check(Num == InitialAutonomousReplicatedTagCounts.Counts.Num());
	const bool bCachedNotifyInstantly = bNotifyTagsUpdateInstantly;
	bNotifyTagsUpdateInstantly = false;
	for (int i = 0; i < Num; i++)
	{
		SetTagCountInternal(InitialAutonomousReplicatedTagCounts.Tags[i], InitialAutonomousReplicatedTagCounts.Counts[i]);
	}
	NotifyTagsUpdated();
	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
}

void UVSGameplayTagFeature::OnRep_InitialSimulationReplicatedTagCounts()
{
	if (!GetOwnerActor() || GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	const int32 Num = InitialSimulationReplicatedTagCounts.Tags.Num();
	check(Num == InitialSimulationReplicatedTagCounts.Counts.Num());
	const bool bCachedNotifyInstantly = bNotifyTagsUpdateInstantly;
	bNotifyTagsUpdateInstantly = false;
	for (int i = 0; i < Num; i++)
	{
		SetTagCountInternal(InitialSimulationReplicatedTagCounts.Tags[i], InitialSimulationReplicatedTagCounts.Counts[i]);
	}
	NotifyTagsUpdated();
	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
}

void UVSGameplayTagFeature::OnAbilitySystemComponentListeningTagsUpdated(const FGameplayTag Tag, int32 Count)
{
	bTagsDirty = true;
	if (bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}
