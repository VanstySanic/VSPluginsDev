// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Framework/VSObjectFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSInteractFeature.generated.h"

class UVSInteractiveActionFeature;
class UVSInteractFeatureAgent;
class UVSInteractiveFeatureAgent;
class UVSGameplayTagController;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Feature.Interact.Base")
class VSINTERACTSYSTEM_API UVSInteractFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSInteractFeatureAgent;
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSInteractFeatureAgent* GetInteractFeatureAgent() const { return InteractFeatureAgentPrivate.Get(); }
	
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSGameplayTagController* GetGameplayTagController() const;

	/** Get the targets witch the agent is currently inspecting. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	TArray<UVSInteractiveFeatureAgent*> GetCurrentInspectiveFeatureAgents() const;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSInteractiveFeatureAgent* GetClosestInspectiveFeatureAgent() const;
	
	/** Get the target witch the agent is currently interacting. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSInteractiveFeatureAgent* GetCurrentInteractiveFeatureAgent() const;

	/** Get the action feature name that the interaction is currently performing. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	FName GetCurrentInteractiveActionFeatureName() const;

	/** Get the action that the interaction is currently performing. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSInteractiveActionFeature* GetCurrentInteractiveActionFeature() const;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	bool IsInspectingOnTarget(UVSInteractiveFeatureAgent* TargetAgent) const;
	
	UFUNCTION(BlueprintCallable, Category = "Interact")
	bool IsInteracting() const;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	float GetInteractionTime() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void OnMovementTagsUpdated();

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void OnMovementTagEventNotified(const FGameplayTag& TagEvent);
	
private:
	TWeakObjectPtr<UVSInteractFeatureAgent> InteractFeatureAgentPrivate;
};
