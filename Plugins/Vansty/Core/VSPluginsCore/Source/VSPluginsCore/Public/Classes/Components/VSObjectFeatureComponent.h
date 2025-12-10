// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VSObjectFeatureComponent.generated.h"

class UVSObjectFeature;

/**
 * Component that owns and manages a hierarchy of UVSObjectFeature instances.
 *
 * - Acts as the root "feature owner" for an Actor.
 * - Responsible for creating / attaching / detaching features.
 * - Optionally auto-registers features during BeginPlay.
 * - Supports replication of the feature list.
 */
UCLASS(ClassGroup = (VS), meta = (BlueprintSpawnableComponent))
class VSPLUGINSCORE_API UVSFeatureComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginDestroy() override;
	//~ End UObject Interface.

	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface.

	/** Register the features if possible. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void RegisterFeatures();

	/** Unregister the features if possible. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void UnregisterFeatures();

	/** Get all sub features recursively. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	TArray<UVSObjectFeature*> GetFeatures() const;

	/** Whether the given feature is contained as a sub feature somewhere under this feature. */
	UFUNCTION(BlueprintCallable, Category = "Features")
	bool HasFeature(UVSObjectFeature* Feature) const;

	/**
	 * Attach an already instanced feature to the component's features.
	 * @param bDeferRegister If true, the feature will be forced not be registered automatically even bRegisterWithOwner is true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void AddFeatureInstance(UVSObjectFeature* Feature, bool bDeferRegister = false);

	/**
	 * Create a new feature instance of the specified class and add it to features.
	 * @param Class Class of the feature to create and attach.
	 * @param bDeferRegister If true, the feature will be forced not be registered automatically even if bRegisterWithOwner is true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	UVSObjectFeature* AddFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bDeferRegister = false);

	/**
	 * Detach and optionally destroy the given feature from this component's feature hierarchy.
	 * Will unregister the feature if it is currently registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Features")
	void RemoveFeature(UVSObjectFeature* Feature);
	
	/** Get the first feature of the given class. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;
	
	/** Get all features of the given class. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	TArray<UVSObjectFeature*> GetFeaturesByClass(TSubclassOf<UVSObjectFeature> Class) const;

	/** Get the first feature with the given not-none name. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetFeatureByName(FName Name) const;
	
	/** Get the first feature of the given class. */
	template <typename T>
	T* FindFeatureByClass(TSubclassOf<T> Class = T::StaticClass()) const;

	/** Get all features of the given class. */
	template <typename T>
	TArray<T*> FindFeaturesByClass(TSubclassOf<T> Class = T::StaticClass()) const;

public:
	/** If true, feature registrations will be executed during BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
	uint8 bRegisterFeaturesDuringBeginPlay : 1;
	
protected:
	/** All direct features owned by this component. May contain nested hierarchies. */
	UPROPERTY(EditAnywhere, Replicated, Instanced, Category = "Features")
	TArray<TObjectPtr<UVSObjectFeature>> Features;
};

template <typename T>
T* UVSFeatureComponent::FindFeatureByClass(TSubclassOf<T> Class) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetFeatureByClass(Class));
}

template <typename T>
TArray<T*> UVSFeatureComponent::FindFeaturesByClass(TSubclassOf<T> Class) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	TArray<T*> OutSubFeatures;
	for (UVSObjectFeature* SubFeature : GetFeaturesByClass(Class))
	{
		OutSubFeatures.Add(static_cast<T*>(SubFeature));
	}
	return OutSubFeatures;
}
