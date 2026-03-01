// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Classes/VSObjectFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSGameplayTagFeature.generated.h"

struct FGameplayTagCountContainer;
struct FGameplayEventData;
class UAbilitySystemComponent;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED)

/**
 * Initial-only snapshot of selected tag counts using parallel tag/count arrays.
 */
USTRUCT()
struct FVSGameplayTagFeatureReplicatedTagCounts
{
	GENERATED_BODY()

	/** Replicated gameplay tags. Tags[i] maps to Counts[i]. */
	UPROPERTY()
	TArray<FGameplayTag> Tags;

	/** Replicated explicit counts for Tags[i]. */
	UPROPERTY()
	TArray<int32> Counts;
};

/**
 * Base gameplay-tag feature with count-based tag state, replication bootstrap,
 * and tag event notifications.
 * Subclasses provide the backing explicit tag container and count map.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, DisplayName = "VS.Feature.GameplayTags.Base")
class VSPLUGINSCORE_API UVSGameplayTagFeatureBase : public UVSObjectFeature, public IGameplayTagAssetInterface
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FGameplayTagsUpdateDelegate, UVSGameplayTagFeatureBase* /** GameplayTagFeature */);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FGameplayTagNotifyDelegate, UVSGameplayTagFeatureBase* /** GameplayTagFeature */, const FGameplayTagContainer& /** TagEvents */);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayTagsUpdateEvent, UVSGameplayTagFeatureBase*, GameplayTagFeature);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameplayTagNotifyEvent, UVSGameplayTagFeatureBase*, GameplayTagFeature, const FGameplayTagContainer&, TagEvents);

public:
	//~ Begin UObject Interface.
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface.

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void BeginPlay_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	//~ End UVSObjectFeature Interface.

public:
	//~ Begin IGameplayTagAssetInterface.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~ End IGameplayTagAssetInterface.

	/** Snapshot of currently owned gameplay tags. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	FGameplayTagContainer GetGameplayTags() const;
	
	/**
	 * Checks if a tag is present, optionally using exact matching.
	 * @param bExact If true, uses explicit tag counts; otherwise uses full count map.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagToCheck"))
	bool HasTag(const FGameplayTag& TagToCheck, bool bExact = true) const;
	
	/**
	 * Returns true if any of the given tags are present.
	 * @param bExact If true, uses HasAnyExact; otherwise uses generic HasAny.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagsToCheck"))
	bool HasAnyTag(const FGameplayTagContainer& TagsToCheck, bool bExact = true) const;
	
	/**
	 * Returns true if all given tags are present.
	 * @param bExact If true, uses HasAllExact; otherwise uses generic HasAll.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagsToCheck"))
	bool HasAllTags(const FGameplayTagContainer& TagsToCheck, bool bExact = true) const;
	
	/**
	 * Returns the count for a given tag.
	 * @param bExact If true, reads explicit count; otherwise reads total count.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagToCheck"))
	int32 GetTagCount(const FGameplayTag& TagToCheck, bool bExact = true) const;

	/**
	 * Checks whether the provided TagQuery matches the owned gameplay tags.
	 * @param bEmptyQueryPass If true, an empty query evaluates as "true".
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagQuery"))
	bool MatchesTagQuery(const FGameplayTagQuery& TagQuery, bool bEmptyQueryPass = false) const;

	/** Checks whether a gameplay tag event query matches a given event set against the currently owned gameplay tags. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "EventQuery, TagEvents"))
	bool MatchesEventQuery(const FVSGameplayTagEventQuery& EventQuery, const FGameplayTagContainer& TagEvents) const;

	/** Increment a single tag count using net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag, NetExecPolicies"))
	void AddTag(const FGameplayTag& GameplayTag, int32 AddCount = 1, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Increment multiple tag counts using net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags, NetExecPolicies"))
	void AddTags(const FGameplayTagContainer& GameplayTags, int32 AddCount = 1, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Decrement a single tag count using net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag, NetExecPolicies"))
	void RemoveTag(const FGameplayTag& GameplayTag, int32 RemoveCount = 1, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Decrement multiple tag counts using net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags, NetExecPolicies"))
	void RemoveTags(const FGameplayTagContainer& GameplayTags, int32 RemoveCount = 1, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Sets explicit count for one tag, applying configured net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag, NetExecPolicies"))
	void SetTagCount(const FGameplayTag& GameplayTag, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Sets explicit count for a tag set, applying configured net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags, NetExecPolicies"))
	void SetTagsCount(const FGameplayTagContainer& GameplayTags, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Returns whether any tag count changes are currently marked as dirty. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	bool IsDirty() const { return bTagsDirty; }

	/** Marks current tag state as dirty without broadcasting updates. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void MarkTagsDirty() { bTagsDirty = true; }

	/**
	 * Marks tags as updated and broadcasts OnTagsUpdated / OnTagEventsNotified.
	 * Intended for local execution.
	 * @param bAllowCleanNotify If true, will notify even when not dirty.
	 * @param OptionalTagEvents Optional event tags to send with the update.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagEvents"))
	void NotifyTagsUpdated(bool bAllowCleanNotify = false, const FGameplayTagContainer& OptionalTagEvents = FGameplayTagContainer());

	/** Triggers a single tag-based event, optionally forwarded via RPC according to net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagEvent, NetExecPolicies"))
	void NotifyTagEvent(const FGameplayTag& TagEvent, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Triggers a container of tag-based events, optionally forwarded via RPC according to net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagEvents, NetExecPolicies"))
	void NotifyTagEvents(const FGameplayTagContainer& TagEvents, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/**
	 * Binds the feature delegates to an object implementing the UVSGameplayTagFeatureInterface callbacks.
	 * @remark Existing bindings for the same object are removed first.
	 * @remark Immediately invokes the interface callbacks once to push the current state.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void BindDelegateForObject(UObject* Object);

	/** Unbinds previously bound delegates from the given object. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void UnbindDelegateForObject(UObject* Object);

	/** Returns a human-readable debug string of current tags and net roles. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	FString GetDebugString();

protected:
	virtual FGameplayTagContainer& GetGameplayTagContainerSourceReference()
		PURE_VIRTUAL(UVSGameplayTagFeatureBase::GetGameplayTagContainerSourceReference, static FGameplayTagContainer Empty; return Empty;);

	virtual TMap<FGameplayTag, int32>& GetGameplayTagCountMapSourceReference()
		PURE_VIRTUAL(UVSGameplayTagFeatureBase::GetGameplayTagCountMapSourceReference, static TMap<FGameplayTag, int32> Empty; return Empty;);

	virtual void ModifyTagCount(const FGameplayTag& GameplayTag, int32 NewCount);
	
private:
	const FGameplayTagContainer& GetGameplayTagContainerSourceConstReference() const;
	const TMap<FGameplayTag, int32>& GetGameplayTagCountMapSourceConstReference() const;
	void InitDefaultGameplayTags();
	void DeltaTagCountInternal(const FGameplayTag& GameplayTag, int32 DeltaCount);
	void DeltaTagsCountInternal(const FGameplayTagContainer& GameplayTags, int32 DeltaCount);
	void SetTagCountInternal(const FGameplayTag& GameplayTag, int32 Count);
	void SetTagsCountInternal(const FGameplayTagContainer& GameplayTags, int32 Count);
	void NotifyTagEventInternal(const FGameplayTag& TagEvent);
	void NotifyTagEventsInternal(const FGameplayTagContainer& TagEvents);
	void UpdateInitialReplicatedTag(const FGameplayTag& GameplayTag);
	void UpdateInitialReplicatedTags(const FGameplayTagContainer& GameplayTags);

	
	UFUNCTION(Server, Reliable)
	void DeltaTagCount_Server(const FGameplayTag& GameplayTag, int32 DeltaCount, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Client, Reliable)
	void DeltaTagCount_Client(const FGameplayTag& GameplayTag, int32 DeltaCount);
	
	UFUNCTION(NetMulticast, Reliable)
	void DeltaTagCount_MultiCast(const FGameplayTag& GameplayTag, int32 DeltaCount);

	UFUNCTION(Server, Reliable)
	void DeltaTagsCount_Server(const FGameplayTagContainer& GameplayTags, int32 DeltaCount, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Client, Reliable)
	void DeltaTagsCount_Client(const FGameplayTagContainer& GameplayTags, int32 DeltaCount);
	
	UFUNCTION(NetMulticast, Reliable)
	void DeltaTagsCount_MultiCast(const FGameplayTagContainer& GameplayTags, int32 DeltaCount);

	UFUNCTION(Server, Reliable)
	void SetTagCount_Server(const FGameplayTag& GameplayTag, int32 Count, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);
	
	UFUNCTION(Client, Reliable)
	void SetTagCount_Client(const FGameplayTag& GameplayTag, int32 Count);
	
	UFUNCTION(NetMulticast, Reliable)
	void SetTagCount_MultiCast(const FGameplayTag& GameplayTag, int32 Count);

	UFUNCTION(Server, Reliable)
	void SetTagsCount_Server(const FGameplayTagContainer& GameplayTags, int32 Count, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Client, Reliable)
	void SetTagsCount_Client(const FGameplayTagContainer& GameplayTags, int32 Count);
	
	UFUNCTION(NetMulticast, Reliable)
	void SetTagsCount_MultiCast(const FGameplayTagContainer& GameplayTags, int32 Count);
	
	UFUNCTION(Server, Reliable)
	void NotifyTagEvent_Server(const FGameplayTag& GameplayTag, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Client, Reliable)
	void NotifyTagEvent_Client(const FGameplayTag& GameplayTag);
	
	UFUNCTION(NetMulticast, Reliable)
	void NotifyTagEvent_MultiCast(const FGameplayTag& GameplayTag);

	UFUNCTION(Server, Reliable)
	void NotifyTagEvents_Server(const FGameplayTagContainer& GameplayTags, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Client, Reliable)
	void NotifyTagEvents_Client(const FGameplayTagContainer& GameplayTags);
	
	UFUNCTION(NetMulticast, Reliable)
	void NotifyTagEvents_MultiCast(const FGameplayTagContainer& GameplayTags);

	UFUNCTION()
	void OnRep_InitialAutonomousReplicatedTagCounts();

	UFUNCTION()
	void OnRep_InitialSimulationReplicatedTagCounts();
	
public:
	/** Broadcast when tag state changes are flushed. */
	FGameplayTagsUpdateDelegate OnTagsUpdated_Native;
	/** Broadcast when gameplay tag events are notified. */
	FGameplayTagNotifyDelegate OnTagEventsNotified_Native;

	/** Broadcast when tag state changes are flushed. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FGameplayTagsUpdateEvent OnTagsUpdated;

	/** Broadcast when gameplay tag events are notified. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FGameplayTagNotifyEvent OnTagEventsNotified;

public:
	/**
	 * If true, OnTagsUpdated broadcasts immediately after each effective tag-count change.
	 * Otherwise, updates remain dirty until NotifyTagsUpdated is called.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bNotifyTagsUpdateInstantly = false;

	/** If true, Tick will flush dirty tag updates via NotifyTagsUpdated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bNotifyTagsUpdateDuringTicks = true;
	
	/** If true, feature will automatically bind delegates to common owner objects during BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bBindDelegatesDuringBeginPlay = true;

	/**
	 * Default tags that will be added to the feature during BeginPlay.
	 * If a tag is here but its count is not assigned in the count map, then the count will be 1 by default.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	FGameplayTagContainer DefaultGameplayTags;
	
	/**
	 * Default tag counts applied during BeginPlay.
	 * If a tag is in DefaultGameplayTags but its count is not assigned here, then the count will be 1 by default.
	 * If a tag exists only in this map, it will still be applied using the mapped count.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	TMap<FGameplayTag, int32> DefaultGameplayTagCounts;

protected:
#if WITH_EDITORONLY_DATA
	/** If true, prints a debug description of owned tags every tick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags|Debug")
	bool bPrintDebugString = false;
#endif
	
private:
	/** Initial tag counts replicated to autonomous proxies (COND_InitialOnly). */
	UPROPERTY(ReplicatedUsing = "OnRep_InitialAutonomousReplicatedTagCounts")
	FVSGameplayTagFeatureReplicatedTagCounts InitialAutonomousReplicatedTagCounts;
	
	/** Initial tag counts replicated to simulated proxies (COND_InitialOnly). */
	UPROPERTY(ReplicatedUsing = "OnRep_InitialSimulationReplicatedTagCounts")
	FVSGameplayTagFeatureReplicatedTagCounts InitialSimulationReplicatedTagCounts;

	/** Cached implicit parent-tag counts derived from explicit tag-count changes. */
	TMap<FGameplayTag, int32> LocalImplicitTagCounts;
	
	/** True when internal tag counts have changed since last NotifyTagsUpdated. */
	uint8 bTagsDirty : 1;
};


/** ------------------------------------------------------------------------- **/


/**
 * Gameplay-tag feature that stores explicit tags/counts locally in this object.
 */
UCLASS(DisplayName = "VS.Feature.GameplayTags.LocalSource")
class UVSLocalSourceGameplayTagFeature : public UVSGameplayTagFeatureBase
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSGameplayTagFeature Interface.
	virtual FGameplayTagContainer& GetGameplayTagContainerSourceReference() override;
	virtual TMap<FGameplayTag, int32>& GetGameplayTagCountMapSourceReference() override;
	//~ End UVSGameplayTagFeature Interface.

private:
	/** Backing explicit tag container for local-source mode. */
	FGameplayTagContainer LocalSourceGameplayTagContainer;
	/** Backing explicit tag count map for local-source mode. */
	TMap<FGameplayTag, int32> LocalSourceGameplayTagCountMap;
};


/** ------------------------------------------------------------------------- **/


/**
 * Gameplay-tag feature backed by an AbilitySystemComponent.
 * Uses the ASC explicit tag container/count map as the authoritative source.
 */
UCLASS(DisplayName = "VS.Feature.GameplayTags.AbilitySystem")
class VSPLUGINSCORE_API UVSAbilitySystemGameplayTagFeature : public UVSGameplayTagFeatureBase
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.

	//~ Begin UVSGameplayTagFeature Interface.
	virtual FGameplayTagContainer& GetGameplayTagContainerSourceReference() override;
	virtual TMap<FGameplayTag, int32>& GetGameplayTagCountMapSourceReference() override;
	virtual void ModifyTagCount(const FGameplayTag& GameplayTag, int32 NewCount) override;
	//~ End UVSGameplayTagFeature Interface.

private:
	UFUNCTION()
	void OnAbilitySystemTagNewOrRemoved(const FGameplayTag Tag, int32 Count);
	
	UFUNCTION()
	void OnAbilitySystemListeningTagCountUpdated(const FGameplayTag Tag, int32 Count);
	
	void OnTagEventsNotified(UVSGameplayTagFeatureBase* Feature, const FGameplayTagContainer& TagEvents);
	void OnAbilitySystemGameplayEventTriggered(FGameplayTag Tag, const FGameplayEventData* EventData);
	
private:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponentPrivate;
	FGameplayTagCountContainer* AbilitySystemGameplayTagCountContainerPtr = nullptr;
	/** Re-entrancy guard for forwarded gameplay events by tag. */
	TMap<FGameplayTag, int32> AbilitySystemBlockedGameplayEventCounts;

	TMap<FGameplayTag, FDelegateHandle> AbilitySystemRegisteredDelegateHandles;
	FDelegateHandle AbilitySystemGameplayEventTagContainerDelegate;
};
