// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSTickableObject.h"
#include "Interfaces/Interface_ActorSubobject.h"
#include "VSObjectFeature.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjectFeature, Log, All);

/**
 * Need the bReplicateUsingRegisteredSubObjectList in the parent actor or component to be true.
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, DisplayName = "Feature.Base")
class VSPLUGINSCORE_API UVSObjectFeature : public UVSTickableObject, public IInterface_ActorSubobject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual void PostInitProperties() override;
	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginDestroy() override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual UWorld* GetWorld() const override;
#if WITH_EDITOR
	virtual bool Modify( bool bAlwaysMarkDirty = true) override;
#endif
	//~ End UObject Interface.

	//~ Begin IInterface_ActorSubobject Interface
	virtual void OnCreatedFromReplication() override;
	virtual void OnDestroyedFromReplication() override;
	//~ End IInterface_ActorSubobject Interface

	
	/** Get the actor that actually owes this feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	AActor* GetOwnerActor() const;
	
	/** Get the component that owes this feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UActorComponent* GetOwnerComponent() const;
	
	/** Get the feature that owes this feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetOwnerFeature() const;

	
	/**
	 * Register the feature, set up the tick function and replication.
	 * This will be automatically called when in blueprint editor, and should be manually called when in game.
	 * This will call the initialize and begin play process.
	 * Will register sub features if possible. Don't call on sub features.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void RegisterFeature();

	/**
	 * Unregister the feature, clean up the tick function and replication.
	 * This will be automatically called and need not be called manually.
	 * This will call the uninitialize and end play process.
	 * Will uninitialize sub features if possible. Don't call on sub features.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void UnregisterFeature();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void BeginPlay();

	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void EndPlay();

public:
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void DestroyFeature();
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool IsRegistered() const { return bIsRegistered; }

	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasBeenInitialized() const { return bHasBeenInitialized; }
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasBegunPlay() const { return bHasBegunPlay; }

	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool IsBeingDestroyed() const { return bIsBeingDestroyed; }

	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void SetIsReplicated(bool bShouldReplicate);
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool GetIsReplicated() const { return bReplicates; }
		
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Feature")
	void RemoveSubFeature(UVSObjectFeature* InFeature, bool bRecursive = true);

	UFUNCTION(BlueprintCallable, Category = "Feature")
	TArray<UVSObjectFeature*> GetSubFeatures(bool bRecursive = true) const;
	
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive = true) const;

	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	TArray<UVSObjectFeature*> GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive = true) const;

	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetSubFeatureByName(FName Name, bool bRecursive = true) const;


	UFUNCTION(BlueprintCallable, Category = "Feature")
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool IsActive() const { return bIsActive; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void OnFeatureActivated();

	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void OnFeatureInactivated();

public:
	template <typename T>
	T* FindSubFeatureByClass(TSubclassOf<T> Class = T::StaticClass(), bool bRecursive = true) const;

	template <typename T>
	TArray<T*> FindSubFeaturesByClass(TSubclassOf<T> Class = T::StaticClass(), bool bRecursive = true) const;

	
private:
	void BeginReplication();
	void EndReplication();
	
	bool AddSubFeatureInternal(UVSObjectFeature* InFeature);

	UFUNCTION()
	void OnRep_bIsActive();

public:
	/** Literal name for the feature to distinguish between features. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feature")
	FName FeatureName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feature")
	bool bAutoActivate = true;
	
private:
	UPROPERTY(EditAnywhere, Instanced, Category = "Feature", meta = (DisplayPriority = 1))
	TArray<TObjectPtr<UVSObjectFeature>> SubFeatures;
	
	/**
	 * Whether this feature currently replicating and should the network code consider it for replication.
	 * Owner actor, component and feature must be replicating first if not none.
	 */
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Feature", DisplayName = "Feature Replicates")
	uint8 bReplicates:1;

	/** Whether the feature is currently active. */
	UPROPERTY(ReplicatedUsing = "OnRep_bIsActive")
	uint8 bIsActive : 1;

	uint8 bIsRegistered: 1;
	uint8 bHasBeenInitialized : 1;
	uint8 bHasBegunPlay : 1;
	uint8 bIsBeingDestroyed : 1;

	/** Actor that owes this feature. */
	TWeakObjectPtr<AActor> OwnerActorPrivate;

	/** Component that owes this feature. */
	TWeakObjectPtr<UActorComponent> OwnerComponentPrivate;

	/** Feature that owes this feature. */
	TWeakObjectPtr<UVSObjectFeature> OwnerFeaturePrivate;
};

template <typename T>
T* UVSObjectFeature::FindSubFeatureByClass(TSubclassOf<T> Class, bool bRecursive) const
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetSubFeatureByClass(Class, bRecursive));
}

template <typename T>
TArray<T*> UVSObjectFeature::FindSubFeaturesByClass(TSubclassOf<T> Class, bool bRecursive) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	TArray<T*> OutSubFeatures;
	for (UVSObjectFeature* SubFeature : GetSubFeaturesByClass(Class, bRecursive))
	{
		OutSubFeatures.Add(static_cast<T*>(SubFeature));
	}
	return OutSubFeatures;
}

