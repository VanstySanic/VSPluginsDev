// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Classes/VSObjectFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSGameplayTagFeature.generated.h"

struct FGameplayTagCountContainer;
class UAbilitySystemComponent;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(VS_PLUGINSCORE_GAMEPAYTAGFEATURE_ONTAGSUPDATED)

/**
 * FVSGameplayTagFeatureReplicatedTagCounts
 *
 * Lightweight replicated container that captures a snapshot of gameplay tag
 * counts using parallel arrays. Intended for initial, one–time replication of
 * selected tag counts to newly joined clients (autonomous / simulated), while
 * runtime tag updates are handled via ASC tag map logic and RPCs.
 *
 * - Tags[i] corresponds to Counts[i]
 * - Only populated for tags configured as "initially replicated" in settings
 * - Replicated with COND_InitialOnly; see OnRep_ handlers for application
 */
USTRUCT()
struct FVSGameplayTagFeatureReplicatedTagCounts
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameplayTag> Tags;

	UPROPERTY()
	TArray<int32> Counts;
};

/**
 * Base feature providing gameplay tag management with count-based semantics.
 *
 * Acts as a feature-level abstraction over a gameplay tag container and its
 * associated count map, supporting network-aware mutation, initial replication,
 * and tag-based event notification.
 *
 * This class does not own the underlying tag storage; subclasses are expected
 * to supply the backing container and count map used for evaluation and mutation.
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

	/** Get the gameplay tags that are owned by the feature's ASC. */
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

	/** Sets the count for a single tag using net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag, NetExecPolicies"))
	void SetTagCount(const FGameplayTag& GameplayTag, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Sets the count for multiple tags using net execution policies. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags, NetExecPolicies"))
	void SetTagsCount(const FGameplayTagContainer& GameplayTags, int32 NewCount, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/** Returns whether any tag count changes are currently marked as dirty. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	bool IsDirty() const { return bTagsDirty; }

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
	FGameplayTagsUpdateDelegate OnTagsUpdated_Native;
	FGameplayTagNotifyDelegate OnTagEventsNotified_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FGameplayTagsUpdateEvent OnTagsUpdated;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FGameplayTagNotifyEvent OnTagEventsNotified;

public:
	/**
	 * If true, OnTagsUpdated delegate will broadcast everytime when a tag count sign changes.
	 * Otherwise, tag update notification should be processed manually.
	 * This might have some cost, so it's recommended not to enable this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bNotifyTagsUpdateInstantly = false;

	/** If true, OnTagsUpdated delegate will broadcast in ticks if dirty. */
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
	 * Default tags counts that will be set to the feature during BeginPlay.
	 * If a tag is in DefaultGameplayTags but its count is not assigned here, then the count will be 1 by default.
	 * If a tag is not in DefaultGameplayTags but in the count map, the tag will also be added with a count of 1.
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

	/** Saves all tags and their parents when tags added or removed. Used to query get tag counts faster. */
	TMap<FGameplayTag, int32> LocalImplicitTagCounts;
	
	/** True when internal tag counts have changed since last NotifyTagsUpdated. */
	uint8 bTagsDirty : 1;
};


/** ------------------------------------------------------------------------- **/


/**
 * UVSLocalSourceGameplayTagFeature
 *
 * A simple gameplay-tag feature that stores its tag container
 * and tag count map locally inside the feature itself.
 *
 * - Provides a self-contained tag source (no external system required).
 * - Useful for lightweight or isolated tag sets that still benefit
 *   from the network-aware APIs and event system in the base class.
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
	FGameplayTagContainer LocalSourceGameplayTagContainer;
	TMap<FGameplayTag, int32> LocalSourceGameplayTagCountMap;
};


/** ------------------------------------------------------------------------- **/


/**
 * UVSAbilitySystemGameplayTagFeature
 *
 * Gameplay-tag feature backed by an AbilitySystemComponent.
 *
 * - Uses the ASC’s internal FGameplayTagCountContainer as the tag source.
 * - Forwards all read/write operations to the ASC’s explicit tag and count maps.
 * - Mirrors ASC tag changes via registered GameplayTagEvent callbacks.
 * - Integrates ASC tag state into the base feature’s event and replication flow.
 *
 * Responsibilities:
 * - Locate and cache the owning ASC on initialize.
 * - Bind to configured ASC tag events and mark the feature dirty on updates.
 * - Use SetTagMapCount() to apply authoritative modifications.
 * - Provide the base class with references to the ASC’s tag container/count map.
 *
 * Suitable when:
 * - The feature should reflect and manipulate real ASC tag state.
 * - You need the base feature’s network-aware APIs and notifications
 *   while retaining the ASC as the authoritative tag storage.
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
	void OnAbilitySystemListeningTagsUpdated(const FGameplayTag Tag, int32 Count);
	
private:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponentPrivate;
	FGameplayTagCountContainer* AbilitySystemGameplayTagCountContainerPtr = nullptr;
	TMap<FGameplayTag, FDelegateHandle> AbilitySystemRegisteredDelegateHandles;
};