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
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetRootFeature() const { return RootFeature; }
	
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* GetRootFeatureByClass(TSubclassOf<UVSObjectFeature> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	void AddInstancedSubFeature(UVSObjectFeature* Feature, FName OptionalFeatureName = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* AddSubFeatureByClass(TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName = NAME_None);

	/** Must call at Outer's constructor, otherwise an error will occur. */
	UFUNCTION(BlueprintCallable, Category = "Feature", meta = (DeterminesOutputType = "Class"))
	UVSObjectFeature* AddDefaultSubFeatureByClass(UObject* Outer, TSubclassOf<UVSObjectFeature> Class, FName OptionalFeatureName = NAME_None);
	
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
	

	template <typename T>
	T* GetRootFeature() const;
	
	template <typename T>
	T* FindSubFeatureByClass(TSubclassOf<T> Class = T::StaticClass(), bool bRecursive = true) const;

	template <typename T>
	TArray<T*> FindSubFeaturesByClass(TSubclassOf<T> Class = T::StaticClass(), bool bRecursive = true) const;
	
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
