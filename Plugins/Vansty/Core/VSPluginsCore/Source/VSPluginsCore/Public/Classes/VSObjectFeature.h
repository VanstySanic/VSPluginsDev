// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSReplicatableObject.h"
#include "Interfaces/VSTickFunctionOwnerInterface.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSObjectFeature.generated.h"

class UVSObjectTickProxy;

DECLARE_LOG_CATEGORY_EXTERN(LogObjectFeature, Log, All);

/**
 * Base instanced feature object attached to an Actor or another feature.
 */
UCLASS(DefaultToInstanced, EditInlineNew, DisplayName = "VS.Feature.Base")
class VSPLUGINSCORE_API UVSObjectFeature : public UVSReplicatableObject, public IVSTickFunctionOwnerInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual void PostInitProperties() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginDestroy() override;
	//~ End UObject Interface.
	
	//~ Begin IVSTickFunctionInterface.
	virtual FTickFunction* GetPrimaryTickFunctionPtr() const override;
	//~ End IVSTickFunctionInterface.
	
protected:
	//~ Begin IInterface_ActorSubobject Interface
	virtual void OnCreatedFromReplication() override;
	virtual void OnDestroyedFromReplication() override;
	//~ End IInterface_ActorSubobject Interface

public:
	/** Returns the owning actor resolved for this feature hierarchy. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	AActor* GetOwnerActor() const;

	/** Returns the direct owner feature in the feature hierarchy. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetOwnerFeature() const { return OwnerFeaturePrivate.Get(); }

	/** Tick proxy used by this feature. Can be null when ticking is not needed. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectTickProxy* GetTickProxy() const { return TickProxy; }
	
	/**
	 * Register the feature, set up the tick function and replication.
	 * This will call the initialize and begin play process, and will register sub features if possible.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void RegisterFeature();

	/**
	 * Unregister the feature, clean up the tick function and replication.
	 * This will call the uninitialize and end play process, and will uninitialize sub features if possible.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void UnregisterFeature();
	
	/** Destroy this feature and its sub features. This will do the unregistration and end play process.*/
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void DestroyFeature();

	/** Runtime registration state set by RegisterFeature / UnregisterFeature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool IsRegistered() const { return bIsRegistered; }

	/** True after Initialize has executed at least once. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasBeenInitialized() const { return bHasBeenInitialized; }

	/** True after BeginPlay has executed. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasBegunPlay() const { return bHasBegunPlay; }

	/** True while destruction/unregistration teardown is in progress. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool IsBeingDestroyed() const { return bIsBeingDestroyed; }
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void SetActive(bool bNewActive);

	/**
	 * Whether the feature is currently active.
	 * Notice that the result only works as a mark and does not affect functionality by default.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool IsActive() const { return bIsActive; }

	UFUNCTION(BlueprintCallable, Category = "Feature")
	TArray<UVSObjectFeature*> GetSubFeatures() const;

	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasSubFeature(UVSObjectFeature* Feature) const;

	/**
	 * Attach an already instanced feature as a sub feature of this feature.
	 * Will re-parent and (re)register the feature if needed.
	 * @param bDeferRegister If true, the feature will be forced not be registered automatically even bRegisterWithOwner is true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void AddSubFeatureInstance(UVSObjectFeature* Feature, bool bDeferRegister = false);

	/**
	 * Create a new feature instance of the specified class and add it as a sub feature.
	 * @param Class Class of the feature to create and attach.
	 * @param bDeferRegister If true, the feature will be forced not be registered automatically even if bRegisterWithOwner is true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bDeferRegister = false);

	/**
	 * Detach and optionally destroy the given sub feature from this feature's hierarchy.
	 * Will unregister the feature if it is currently registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void RemoveSubFeature(UVSObjectFeature* Feature);

	/**
	 * Set a new owner feature for this feature.
	 * This will re-parent the feature in the hierarchy and adjust registration if necessary.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void SetOwnerFeature(UVSObjectFeature* Feature, bool bDeferRegister = false);

	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasOwnerFeature(UVSObjectFeature* Feature) const;

	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetOwnerFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;
	
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	TArray<UVSObjectFeature*> GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetSubFeatureByName(FName Name) const;
	
	template <typename T>
	T* FindSubFeatureByClass(TSubclassOf<T> Class = T::StaticClass()) const;

	template <typename T>
	TArray<T*> FindSubFeaturesByClass(TSubclassOf<T> Class = T::StaticClass()) const;
	
	template <typename T>
	T* FindOwnerFeatureByClass(TSubclassOf<T> Class = T::StaticClass()) const;

protected:
	/** Feature lifecycle hook. Called during RegisterFeature before BeginPlay. */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Initialize();

	/** Feature lifecycle hook. Called during UnregisterFeature teardown. */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Uninitialize();
	
	/** Feature lifecycle hook mapped to play start for this feature. */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void BeginPlay();

	/** Feature lifecycle hook mapped to play end for this feature. */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void EndPlay();

	/** Enables/disables the internal tick proxy registration state. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void SetTickEnabled(bool bEnabled);

	/** Per-frame gate for Tick execution. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Feature")
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
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Tick(float DeltaTime);

protected:
	/** Called when active flag transitions to true. */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void OnFeatureActivated();

	/** Called when active flag transitions to false. */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void OnFeatureDeactivated();
	
private:
	/** Tick-proxy bridge that forwards proxy ticks into TickFeature/Tick. */
	void ReceiveProxyTick(UVSObjectTickProxy* PrimaryTickProxy, float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction);
	
	UFUNCTION()
	void OnRep_bIsActive();

	UFUNCTION()
	void OnRep_OwnerFeaturePrivate(UVSObjectFeature* PrevOwnerFeature);
	
public:
	/** Literal name for the feature to distinguish between features. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feature")
	FName FeatureName = NAME_None;

	/** If true, feature auto-activates after BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feature")
	uint8 bAutoActivate : 1;

	/**
	 * If true, feature will be registered when its owner feature is registered or reparenting.
	 * Otherwise, you should manually register the feature.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feature")
	uint8 bRegisterWithOwner : 1;

protected:
	/** All direct sub features owned by this feature. May contain nested hierarchies. */
	UPROPERTY(EditAnywhere, Replicated, Instanced, Category = "Feature")
	TArray<TObjectPtr<UVSObjectFeature>> SubFeatures;

	/**
	 * The proxy that actually handles ticking.
	 * If your class needs no ticking method, you can set this to null to save some memory.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Feature")
	TObjectPtr<UVSObjectTickProxy> TickProxy;

private:
	/** Direct owner feature in the feature hierarchy. */
	UPROPERTY(ReplicatedUsing = "OnRep_OwnerFeaturePrivate")
	TObjectPtr<UVSObjectFeature> OwnerFeaturePrivate;
	
	/**
	 * Whether the feature is currently active.
	 * Notice that this is only a mark and does not affect functionality by default.
	 */
	UPROPERTY(ReplicatedUsing = "OnRep_bIsActive")
	uint8 bIsActive : 1;
	
	/** Internal lifecycle flags. */
	uint8 bIsRegistered: 1;
	uint8 bHasBeenInitialized : 1;
	uint8 bHasBegunPlay : 1;
	uint8 bIsBeingDestroyed : 1;

	/** Cached owner actor pointer for fast access. */
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
