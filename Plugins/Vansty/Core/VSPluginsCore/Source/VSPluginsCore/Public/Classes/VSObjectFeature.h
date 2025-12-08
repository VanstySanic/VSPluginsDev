// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSReplicatableObject.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSObjectFeature.generated.h"

class UVSObjectTickProxy;

DECLARE_LOG_CATEGORY_EXTERN(LogObjectFeature, Log, All);

/**
 * UVSGameplayTagFeature
 *
 * A flexible, replicatable, and ASC-compatible gameplay-tag management feature
 * that allows any Actor to own, modify, and react to GameplayTags with full
 * count-based semantics — whether or not the Actor uses an AbilitySystemComponent.
 *
 * -------------------------------------------------------------------------
 * Core Functionality
 * -------------------------------------------------------------------------
 * - Tag ownership:
 *   Supports explicit & implicit tag tracking with count-based add/remove/set.
 *
 * - Hybrid source model:
 *   Tags may be stored locally or pulled directly from an ASC’s internal
 *   FGameplayTagCountContainer when bUseAbilitySystemComponentSource is enabled.
 *
 * - Replication model:
 *   - Supports InitialOnly replication for predefined autonomous/simulated tags.
 *   - Rich RPC policy system (Client/Server/Multicast) for tag modification.
 *   - Explicit and implicit tag counts kept consistent across network roles.
 *
 * - Event dispatching:
 *   - OnTagsUpdated (count changes)
 *   - OnTagEventsNotified (user-defined tag events)
 *   Includes native + Blueprint multicast delegates and optional auto-binding.
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * - Add/remove/set tag counts locally or through RPC policies.
 * - Bind other objects to tag update events (auto or manual).
 * - Use MatchesTagQuery / MatchesEventQuery for hierarchical logic.
 * - Use optional automatic tick-based notifications.
 */
UCLASS(DefaultToInstanced, EditInlineNew, DisplayName = "VS.Feature.Base")
class VSPLUGINSCORE_API UVSObjectFeature : public UVSReplicatableObject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual void PostInitProperties() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginDestroy() override;
	//~ End UObject Interface.
	
protected:
	//~ Begin IInterface_ActorSubobject Interface
	virtual void OnCreatedFromReplication() override;
	virtual void OnDestroyedFromReplication() override;
	//~ End IInterface_ActorSubobject Interface

public:
	/** Get the actor that actually owes this feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	AActor* GetOwnerActor() const;
	
	/** Get the feature that directly contain this feature as sub feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetOwnerFeature() const { return OwnerFeaturePrivate.Get(); }

	/** Get the tick proxy that actually performs ticking for this feature. Can be null if ticking is not required. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	UVSObjectTickProxy* GetTickProxy() const { return TickProxy; }
	
	/**
	 * Register the feature, set up the tick function and replication.
	 * This will call the initialize and begin play process, and will register sub features if possible.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void RegisterFeature();

	/**
	 * Unregister the feature, clean up the tick function and replication.
	 * This will call the uninitialize and end play process, and will uninitialize sub features if possible.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void UnregisterFeature();
	
	/** Destroy this feature and its sub features. This will do the unregistration and end play process.*/
	UFUNCTION(BlueprintCallable, Category = "Features")
	void DestroyFeature();

	UFUNCTION(BlueprintCallable, Category = "Features")
	bool IsRegistered() const { return bIsRegistered; }

	UFUNCTION(BlueprintCallable, Category = "Features")
	bool HasBeenInitialized() const { return bHasBeenInitialized; }

	UFUNCTION(BlueprintCallable, Category = "Features")
	bool HasBegunPlay() const { return bHasBegunPlay; }

	UFUNCTION(BlueprintCallable, Category = "Features")
	bool IsBeingDestroyed() const { return bIsBeingDestroyed; }
	
	UFUNCTION(BlueprintCallable, Category = "Features")
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "Features")
	bool IsActive() const { return bIsActive; }

	/** Get all sub features recursively. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	TArray<UVSObjectFeature*> GetSubFeatures() const;

	/** Whether the given feature is contained as a sub feature somewhere under this feature. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	bool HasSubFeature(UVSObjectFeature* Feature) const;

	/**
	 * Attach an already instanced feature as a sub feature of this feature.
	 * Will re-parent and (re)register the feature if needed.
	 * @param bDeferRegister If true, the feature will be forced not be registered automatically even bRegisterWithOwner is true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void AddSubFeatureInstance(UVSObjectFeature* Feature, bool bDeferRegister = false);

	/**
	 * Create a new feature instance of the specified class and add it as a sub feature.
	 * @param Class Class of the feature to create and attach.
	 * @param bDeferRegister If true, the feature will be forced not be registered automatically even if bRegisterWithOwner is true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	UVSObjectFeature* AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bDeferRegister = false);

	/**
	 * Detach and optionally destroy the given sub feature from this feature's hierarchy.
	 * Will unregister the feature if it is currently registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void RemoveSubFeature(UVSObjectFeature* Feature);

	/**
	 * Set a new owner feature for this feature.
	 * This will re-parent the feature in the hierarchy and adjust registration if necessary.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void SetOwnerFeature(UVSObjectFeature* Feature, bool bDeferRegister = false);

	/** Whether the given feature appears in this feature's owner chain. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	bool HasOwnerFeature(UVSObjectFeature* Feature) const;

	/** Get the nearest owner feature of the given class. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetOwnerFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;

	/** Get the first sub feature of the given class. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;
	
	/** Get all sub features of the given class. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	TArray<UVSObjectFeature*> GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class) const;

	/** Get the first sub-feature with the given not-none name. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetSubFeatureByName(FName Name) const;
	
	/** Get the first sub feature of the given class. */
	template <typename T>
	T* FindSubFeatureByClass(TSubclassOf<T> Class = T::StaticClass()) const;

	/** Get all sub features of the given class. */
	template <typename T>
	TArray<T*> FindSubFeaturesByClass(TSubclassOf<T> Class = T::StaticClass()) const;
	
	/** Get the first sub-feature with the given not-none name. */
	template <typename T>
	T* FindOwnerFeatureByClass(TSubclassOf<T> Class = T::StaticClass()) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void BeginPlay();

	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void EndPlay();

	UFUNCTION(BlueprintCallable, Category = "Features")
	void SetTickEnabled(bool bEnabled);

	/** Whether the tick function can be executed at this frame. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Features")
	bool CanTick() const;
	
	/**
	 * Function called every frame on this feature.
	 * This will call the Tick method.
	 * Only executes if the TickProxy is valid and registered.
	 * @param DeltaTime The time since the last tick.
	 * @param TickType The kind of tick this is, for example, are we paused, or 'simulating' in the editor
	 * @param TickFunction Internal tick function struct that caused this to run
	 */
	virtual void TickFeature(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction);
	
	/**
	 * Function called every frame on this feature.
	 * Override this function to implement custom logic to be executed every frame.
	 * Only executes if the TickProxy is valid and registered.
	 * @param DeltaTime The time since the last tick.
	 * @remarks You might need to call the super native implementation manually in blueprints.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void Tick(float DeltaTime);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void OnFeatureActivated();

	UFUNCTION(BlueprintNativeEvent, Category = "Features")
	void OnFeatureDeactivated();
	
private:
	UFUNCTION()
	void OnRep_bIsActive();

	UFUNCTION()
	void OnRep_OwnerFeaturePrivate(UVSObjectFeature* PrevOwnerFeature);
	
public:
	/** Literal name for the feature to distinguish between features. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
	FName FeatureName = NAME_None;

	/** Whether the feature should activate automatically after BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
	uint8 bAutoActivate : 1;

	/**
	 * If true, feature will be registered when its owner feature is registered or reparenting.
	 * Otherwise, you should manually register the feature.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
	uint8 bRegisterWithOwner : 1;

protected:
	/** All direct sub features owned by this feature. May contain nested hierarchies. */
	UPROPERTY(EditAnywhere, Replicated, Instanced, Category = "Features")
	TArray<TObjectPtr<UVSObjectFeature>> SubFeatures;

	/**
	 * The proxy that actually handles ticking.
	 * If your class needs no ticking method, you can set this to null to save some memory.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Features")
	TObjectPtr<UVSObjectTickProxy> TickProxy;

private:
	/** Feature that owes this as sub feature. */
	UPROPERTY(ReplicatedUsing = "OnRep_OwnerFeaturePrivate")
	TObjectPtr<UVSObjectFeature> OwnerFeaturePrivate;
	
	/** Whether the feature is currently active. */
	UPROPERTY(ReplicatedUsing = "OnRep_bIsActive")
	uint8 bIsActive : 1;
	
	uint8 bIsRegistered: 1;
	uint8 bHasBeenInitialized : 1;
	uint8 bHasBegunPlay : 1;
	uint8 bIsBeingDestroyed : 1;

	/** Actor that owes this feature. */
	TWeakObjectPtr<AActor> OwnerActorPrivate;
};

template <typename T>
T* UVSObjectFeature::FindSubFeatureByClass(TSubclassOf<T> Class) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetSubFeatureByClass(Class));
}

template <typename T>
TArray<T*> UVSObjectFeature::FindSubFeaturesByClass(TSubclassOf<T> Class) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	TArray<T*> OutSubFeatures;
	for (UVSObjectFeature* SubFeature : GetSubFeaturesByClass(Class))
	{
		OutSubFeatures.Add(static_cast<T*>(SubFeature));
	}
	return OutSubFeatures;
}

template <typename T>
T* UVSObjectFeature::FindOwnerFeatureByClass(TSubclassOf<T> Class) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetOwnerFeatureByClass(Class));
}