// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VSObjectFeatureComponent.generated.h"

class UVSObjectFeature;

/**
 * Allows user to use a root feature and manage its sub features through the component.
 */
UCLASS(ClassGroup = "Gameplay", meta = (BlueprintSpawnableComponent))
class VSPLUGINSCORE_API UVSObjectFeatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVSObjectFeatureComponent();
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	/** Get root feature of this component. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetRootFeature() const { return RootFeature; }

	// /** Get root feature if it matches the given class. */
	// UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	// UVSObjectFeature* GetRootFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;

	/** Add an instanced sub feature to the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	void AddInstancedSubFeature(UVSObjectFeature* Feature, FName OptionalFeatureName = NAME_None);

	/** Create and add a sub feature by class to the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName = NAME_None);

	/**
	 * Create and add a default sub feature by class.
	 * Must be called in the owner's constructor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* AddDefaultSubFeatureByClass(UObject* Outer, TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName = NAME_None);

	/** Remove a sub feature from the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void RemoveSubFeature(UVSObjectFeature* InFeature, bool bRecursive = true);

	/** Get all sub features from the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	TArray<UVSObjectFeature*> GetSubFeatures(bool bRecursive = true) const;

	/** Get the first sub feature of a given class from the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive = true) const;

	/** Get all sub features of a given class from the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	TArray<UVSObjectFeature*> GetSubFeaturesByClass(TSubclassOf<UVSObjectFeature> Class, bool bRecursive = true) const;

	/** Get sub feature by name from the root feature. */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	UVSObjectFeature* GetSubFeatureByName(FName Name, bool bRecursive = true) const;
	
public:
	/** Get root feature of specific type. */
	template <typename T>
	T* GetRootFeature() const;

	/** Find the first sub feature of given class. */
	template <typename T>
	T* FindSubFeatureByClass(TSubclassOf<T> Class = T::StaticClass(), bool bRecursive = true) const;

	/** Find all sub features of given class. */
	template <typename T>
	TArray<T*> FindSubFeaturesByClass(TSubclassOf<T> Class = T::StaticClass(), bool bRecursive = true) const;

public:
	/**
	 * If true, root feature will register during BeginPlay.
	 * If your feature concerns about PlayerState, then this is suggested to be false.
	 * In that case, you should register root feature manually during OnPlayerStateChange or OnRep_PlayerState.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Feature")
	bool bRegisterOnBeginPlay = true;
	
private:
	UPROPERTY(EditAnywhere, Instanced, Category = "Feature")
	TObjectPtr<UVSObjectFeature> RootFeature;
};


template <typename T>
T* UVSObjectFeatureComponent::GetRootFeature() const
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(RootFeature);
}

template <typename T>
T* UVSObjectFeatureComponent::FindSubFeatureByClass(TSubclassOf<T> Class, bool bRecursive) const
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "Class must derive from UVSObjectFeature.");
	return static_cast<T*>(GetSubFeatureByClass(Class, bRecursive));
}

template <typename T>
TArray<T*> UVSObjectFeatureComponent::FindSubFeaturesByClass(TSubclassOf<T> Class, bool bRecursive) const
{
	static_assert(TIsDerivedFrom<T, UVSObjectFeature>::IsDerived, "Class must derive from UVSObjectFeature.");
	TArray<T*> OutSubFeatures;
	for (UVSObjectFeature* SubFeature : GetSubFeaturesByClass(Class, bRecursive))
	{
		OutSubFeatures.Add(static_cast<T*>(SubFeature));
	}
	return OutSubFeatures;
}
