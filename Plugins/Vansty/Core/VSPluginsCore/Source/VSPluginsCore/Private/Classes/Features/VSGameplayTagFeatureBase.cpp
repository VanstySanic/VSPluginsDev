// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Settings/VSPluginsCoreGameSettings.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Net/UnrealNetwork.h"
#include "Types/VSGameQueryTypes.h"

static FGameplayTagContainer EmptyGameplayTagContainer = FGameplayTagContainer();
static TMap<FGameplayTag, int32> EmptyGameplayTagCountMap = TMap<FGameplayTag, int32>();

UE_DEFINE_GAMEPLAY_TAG(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED, "VS.Feature.GameplayTags.OnTagsUpdated")

UVSGameplayTagFeatureBase::UVSGameplayTagFeatureBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
}

void UVSGameplayTagFeatureBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != nullptr) { BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps); }

	DOREPLIFETIME_CONDITION(UVSGameplayTagFeatureBase, InitialAutonomousReplicatedTagCounts, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UVSGameplayTagFeatureBase, InitialSimulationReplicatedTagCounts, COND_InitialOnly);
}

void UVSGameplayTagFeatureBase::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	InitDefaultGameplayTags();

	/** Refresh from replication for safety. */
	OnRep_InitialAutonomousReplicatedTagCounts();
	OnRep_InitialSimulationReplicatedTagCounts();
	
	NotifyTagsUpdated();

	/** Auto bind delegates. */
	if (bBindDelegatesDuringBeginPlay)
	{
		BindDelegateForObject(GetOwnerActor());
	}
}

void UVSGameplayTagFeatureBase::Tick_Implementation(float DeltaTime)
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

void UVSGameplayTagFeatureBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagContainerSourceReference();
}

bool UVSGameplayTagFeatureBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagContainerSourceReference().HasTag(TagToCheck);
}

bool UVSGameplayTagFeatureBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagContainerSourceReference().HasAll(TagContainer);
}

bool UVSGameplayTagFeatureBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagContainerSourceReference().HasAny(TagContainer);
}

FGameplayTagContainer UVSGameplayTagFeatureBase::GetGameplayTags() const
{
	return const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagContainerSourceReference();
}

const FGameplayTagContainer& UVSGameplayTagFeatureBase::GetGameplayTagContainerSourceConstReference() const
{
	return const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagContainerSourceReference();
}

const TMap<FGameplayTag, int32>& UVSGameplayTagFeatureBase::GetGameplayTagCountMapSourceConstReference() const
{
	return const_cast<UVSGameplayTagFeatureBase*>(this)->GetGameplayTagCountMapSourceReference();
}

void UVSGameplayTagFeatureBase::InitDefaultGameplayTags()
{
	for (const FGameplayTag& GameplayTag : DefaultGameplayTags.GetGameplayTagArray())
	{
		if (!DefaultGameplayTagCounts.Contains(GameplayTag))
		{
			DefaultGameplayTagCounts.Add(GameplayTag, 1);
		}
	}

	/** Filter the map to skip replicated tags. */
	TMap<FGameplayTag, int> FilteredDefaultMap = DefaultGameplayTagCounts;
	for (const TPair<FGameplayTag, int>& GameplayTagCountMap : DefaultGameplayTagCounts)
	{
		bool bShouldRemove = false;
		if (GameplayTagCountMap.Value <= 0)
		{
			bShouldRemove = true;
		}
		else if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
		{
			if (UVSPluginsCoreGameSettings::Get()->InitialAutonomousReplicatedGameplayTags.HasTagExact(GameplayTagCountMap.Key))
			{
				bShouldRemove = true;
			}
		}
		else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
		{
			if (UVSPluginsCoreGameSettings::Get()->InitialSimulationReplicatedGameplayTags.HasTagExact(GameplayTagCountMap.Key))
			{
				bShouldRemove = true;
			}
		}
		if (bShouldRemove)
		{
			FilteredDefaultMap.Remove(GameplayTagCountMap.Key);
		}
	}

	if (!FilteredDefaultMap.IsEmpty())
	{
		bool bCachedNotifyInstantly = bNotifyTagsUpdateInstantly;
		bNotifyTagsUpdateInstantly = false;
		
		for (const TPair<FGameplayTag, int>& DefaultMap : FilteredDefaultMap)
		{
			SetTagCountInternal(DefaultMap.Key, DefaultMap.Value);
		}
		
		bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
		if (bNotifyTagsUpdateInstantly)
		{
			NotifyTagsUpdated();
		}
	}
}

bool UVSGameplayTagFeatureBase::HasTag(const FGameplayTag& TagToCheck, bool bExact) const
{
	return bExact ? GetGameplayTagCountMapSourceConstReference().Contains(TagToCheck) : LocalImplicitTagCounts.Contains(TagToCheck);
}

bool UVSGameplayTagFeatureBase::HasAnyTag(const FGameplayTagContainer& TagsToCheck, bool bExact) const
{
	return bExact ? GetGameplayTagContainerSourceConstReference().HasAnyExact(TagsToCheck) : GetGameplayTagContainerSourceConstReference().HasAny(TagsToCheck);
}

bool UVSGameplayTagFeatureBase::HasAllTags(const FGameplayTagContainer& TagsToCheck, bool bExact) const
{
	return bExact ? GetGameplayTagContainerSourceConstReference().HasAllExact(TagsToCheck) : GetGameplayTagContainerSourceConstReference().HasAll(TagsToCheck);
}

int32 UVSGameplayTagFeatureBase::GetTagCount(const FGameplayTag& TagToCheck, bool bExact) const
{
	return bExact ? GetGameplayTagCountMapSourceConstReference().FindRef(TagToCheck) : LocalImplicitTagCounts.FindRef(TagToCheck);
}

bool UVSGameplayTagFeatureBase::MatchesTagQuery(const FGameplayTagQuery& TagQuery, bool bEmptyQueryPass) const
{
	if (TagQuery.IsEmpty() && bEmptyQueryPass) return true;
	return TagQuery.Matches(GetGameplayTagContainerSourceConstReference());
}

bool UVSGameplayTagFeatureBase::MatchesEventQuery(const FVSGameplayTagEventQuery& EventQuery, const FGameplayTagContainer& TagEvents) const
{
	return EventQuery.Matches(TagEvents, GetGameplayTagContainerSourceConstReference());
}

void UVSGameplayTagFeatureBase::AddTag(const FGameplayTag& GameplayTag, int32 AddCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::AddTags(const FGameplayTagContainer& GameplayTags, int32 AddCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::RemoveTag(const FGameplayTag& GameplayTag, int32 RemoveCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::RemoveTags(const FGameplayTagContainer& GameplayTags, int32 RemoveCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::SetTagCount(const FGameplayTag& GameplayTag, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::SetTagsCount(const FGameplayTagContainer& GameplayTags, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::NotifyTagsUpdated(bool bAllowCleanNotify, const FGameplayTagContainer& OptionalTagEvents)
{
	if (!bAllowCleanNotify && !IsDirty()) return;
	bTagsDirty = false;
	OnTagsUpdated_Native.Broadcast(this);
	OnTagsUpdated.Broadcast(this);
	
	FGameplayTagContainer TagEvents = OptionalTagEvents;
	TagEvents.AddTag(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED);
	NotifyTagEvents(TagEvents);
}

void UVSGameplayTagFeatureBase::NotifyTagEvent(const FGameplayTag& TagEvent, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::NotifyTagEvents(const FGameplayTagContainer& TagEvents, const FVSNetMethodExecutionPolicies& NetExecPolicies)
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

void UVSGameplayTagFeatureBase::BindDelegateForObject(UObject* Object)
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

void UVSGameplayTagFeatureBase::UnbindDelegateForObject(UObject* Object)
{
	if (!Object || !Object->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass())) return;
	
	TScriptDelegate Delegate;
	Delegate.BindUFunction(Object, "OnGameplayTagFeatureTagsUpdated");
	OnTagsUpdated.Remove(Delegate);
	
	Delegate.Unbind();
	Delegate.BindUFunction(Object, "OnGameplayTagFeatureTagEventsNotified");
	OnTagEventsNotified.Remove(Delegate);
}

FString UVSGameplayTagFeatureBase::GetDebugString()
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

FGameplayTagContainer& UVSGameplayTagFeatureBase::GetGameplayTagContainerSourceReference()
{
	return EmptyGameplayTagContainer;
}

TMap<FGameplayTag, int32>& UVSGameplayTagFeatureBase::GetGameplayTagCountMapSourceReference()
{
	return EmptyGameplayTagCountMap;
}

void UVSGameplayTagFeatureBase::ModifyTagCount(const FGameplayTag& GameplayTag, int32 NewCount)
{
	const int32 PrevCount = GetTagCount(GameplayTag);
	const int32 DesiredCount = FMath::Max(0, NewCount);
	if (PrevCount == DesiredCount) return;
	
	if (PrevCount == 0 && DesiredCount > 0)
	{
		GetGameplayTagContainerSourceReference().AddTagFast(GameplayTag);
		GetGameplayTagCountMapSourceReference().FindOrAdd(GameplayTag) = DesiredCount;
	}
	else if (PrevCount > 0 && DesiredCount == 0)
	{
		GetGameplayTagContainerSourceReference().RemoveTag(GameplayTag);
		GetGameplayTagCountMapSourceReference().Remove(GameplayTag);
	}
	else if (DesiredCount > 0)
	{
		GetGameplayTagCountMapSourceReference().FindOrAdd(GameplayTag) = DesiredCount;
	}
}

void UVSGameplayTagFeatureBase::DeltaTagCountInternal(const FGameplayTag& GameplayTag, int32 DeltaCount)
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

void UVSGameplayTagFeatureBase::DeltaTagsCountInternal(const FGameplayTagContainer& GameplayTags, int32 DeltaCount)
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

void UVSGameplayTagFeatureBase::SetTagCountInternal(const FGameplayTag& GameplayTag, int32 Count)
{
	const int32 PrevCount = GetTagCount(GameplayTag);
	const int32 DesiredCount = FMath::Max(0, Count);
	if (DesiredCount == PrevCount) return;

	ModifyTagCount(GameplayTag, Count);
	
	const int32 CurrentCount = GetTagCount(GameplayTag);
	if (CurrentCount == PrevCount) return;

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
		
	bTagsDirty = true;
		
	UpdateInitialReplicatedTag(GameplayTag);
	if (bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}

void UVSGameplayTagFeatureBase::SetTagsCountInternal(const FGameplayTagContainer& GameplayTags, int32 Count)
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

void UVSGameplayTagFeatureBase::UpdateInitialReplicatedTag(const FGameplayTag& GameplayTag)
{
	if (!GetOwnerActor()->HasAuthority()) return;
	const int32 CurrentCount = GetTagCount(GameplayTag);
	if (UVSPluginsCoreGameSettings::Get()->InitialAutonomousReplicatedGameplayTags.HasTagExact(GameplayTag))
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

	if (UVSPluginsCoreGameSettings::Get()->InitialSimulationReplicatedGameplayTags.HasTagExact(GameplayTag))
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

void UVSGameplayTagFeatureBase::UpdateInitialReplicatedTags(const FGameplayTagContainer& GameplayTags)
{
	for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray())
	{
		UpdateInitialReplicatedTag(GameplayTag);	
	}
}

void UVSGameplayTagFeatureBase::NotifyTagEventInternal(const FGameplayTag& TagEvent)
{
	if (TagEvent == FGameplayTag::EmptyTag) return;
	NotifyTagEventsInternal(FGameplayTagContainer(TagEvent));
}

void UVSGameplayTagFeatureBase::NotifyTagEventsInternal(const FGameplayTagContainer& TagEvents)
{
	OnTagEventsNotified_Native.Broadcast(this,TagEvents);
	OnTagEventsNotified.Broadcast(this, TagEvents);
}

void UVSGameplayTagFeatureBase::DeltaTagCount_Server_Implementation(const FGameplayTag& GameplayTag, int32 DeltaCount, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
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

void UVSGameplayTagFeatureBase::DeltaTagCount_Client_Implementation(const FGameplayTag& GameplayTag, int32 DeltaCount)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	DeltaTagCountInternal(GameplayTag, DeltaCount);
}

void UVSGameplayTagFeatureBase::DeltaTagCount_MultiCast_Implementation(const FGameplayTag& GameplayTag, int32 DeltaCount)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	DeltaTagCountInternal(GameplayTag, DeltaCount);
}

void UVSGameplayTagFeatureBase::DeltaTagsCount_Server_Implementation(const FGameplayTagContainer& GameplayTags, int32 DeltaCount, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
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

void UVSGameplayTagFeatureBase::DeltaTagsCount_Client_Implementation(const FGameplayTagContainer& GameplayTags, int32 DeltaCount)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	DeltaTagsCountInternal(GameplayTags, DeltaCount);
}

void UVSGameplayTagFeatureBase::DeltaTagsCount_MultiCast_Implementation(const FGameplayTagContainer& GameplayTags, int32 DeltaCount)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	DeltaTagsCountInternal(GameplayTags, DeltaCount);
}

void UVSGameplayTagFeatureBase::SetTagCount_Server_Implementation(const FGameplayTag& GameplayTag, int32 Count, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
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

void UVSGameplayTagFeatureBase::SetTagCount_Client_Implementation(const FGameplayTag& GameplayTag, int32 Count)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	SetTagCountInternal(GameplayTag, Count);
}

void UVSGameplayTagFeatureBase::SetTagCount_MultiCast_Implementation(const FGameplayTag& GameplayTag, int32 Count)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	SetTagCountInternal(GameplayTag, Count);
}

void UVSGameplayTagFeatureBase::SetTagsCount_Server_Implementation(const FGameplayTagContainer& GameplayTags, int32 Count, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
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

void UVSGameplayTagFeatureBase::SetTagsCount_Client_Implementation(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	SetTagsCountInternal(GameplayTags, Count);
}

void UVSGameplayTagFeatureBase::SetTagsCount_MultiCast_Implementation(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	SetTagsCountInternal(GameplayTags, Count);
}

void UVSGameplayTagFeatureBase::NotifyTagEvent_Server_Implementation(const FGameplayTag& GameplayTag, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
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

void UVSGameplayTagFeatureBase::NotifyTagEvent_Client_Implementation(const FGameplayTag& GameplayTag)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	NotifyTagEventInternal(GameplayTag);
}

void UVSGameplayTagFeatureBase::NotifyTagEvent_MultiCast_Implementation(const FGameplayTag& GameplayTag)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	NotifyTagEventInternal(GameplayTag);
}

void UVSGameplayTagFeatureBase::NotifyTagEvents_Server_Implementation(const FGameplayTagContainer& GameplayTags, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
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

void UVSGameplayTagFeatureBase::NotifyTagEvents_Client_Implementation(const FGameplayTagContainer& GameplayTags)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_AutonomousProxy) return;
	NotifyTagEventsInternal(GameplayTags);
}

void UVSGameplayTagFeatureBase::NotifyTagEvents_MultiCast_Implementation(const FGameplayTagContainer& GameplayTags)
{
	if (GetOwnerActor()->GetLocalRole() != ROLE_SimulatedProxy) return;
	NotifyTagEventsInternal(GameplayTags);
}

void UVSGameplayTagFeatureBase::OnRep_InitialAutonomousReplicatedTagCounts()
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
	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
	if (bNotifyTagsUpdateInstantly)
	{
		NotifyTagsUpdated();
	}
}

void UVSGameplayTagFeatureBase::OnRep_InitialSimulationReplicatedTagCounts()
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
	bNotifyTagsUpdateInstantly = bCachedNotifyInstantly;
	if (bCachedNotifyInstantly)
	{
		NotifyTagsUpdated();
	}
}


/** ------------------------------------------------------------------------- **/


UVSLocalSourceGameplayTagFeature::UVSLocalSourceGameplayTagFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayTagContainer& UVSLocalSourceGameplayTagFeature::GetGameplayTagContainerSourceReference()
{
	return LocalSourceGameplayTagContainer;
}

TMap<FGameplayTag, int32>& UVSLocalSourceGameplayTagFeature::GetGameplayTagCountMapSourceReference()
{
	return LocalSourceGameplayTagCountMap;
}
