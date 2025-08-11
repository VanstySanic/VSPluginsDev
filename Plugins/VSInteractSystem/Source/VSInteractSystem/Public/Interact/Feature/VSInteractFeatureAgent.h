// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSInteractFeature.h"
#include "VSInteractFeatureAgent.generated.h"

class UVSInteractiveFeature;

/**
 * 
 */
UCLASS(DisplayName = "Feature.Interact.Agent")
class VSINTERACTSYSTEM_API UVSInteractFeatureAgent : public UVSInteractFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSInteractFeature;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInspectSignature, UVSInteractiveFeatureAgent*, TargetAgent);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractSignature, UVSInteractiveFeatureAgent*, TargetAgent, const FName, ActionFeatureName);
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void EndPlay_Implementation() override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	/** Inspect target interactive object. This normally only works on local clients to help show tip widgets. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void TryInspectTarget(UVSInteractiveFeatureAgent* TargetAgent);

	/** End the inspection on target. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void StopInspectionOnTarget(UVSInteractiveFeatureAgent* TargetAgent);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void StopAllInspections();
	
	/**
	 * Interact with target interactive object and use the specified action.
	 * Notice that the net execution policies will follow the action's.
	 * @param ActionFeatureName The action's feature name. If left none, will use the first action.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void TryInteractWithTarget(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName = NAME_None);

	/**
	 * End the interaction with target.
	 * @param ActionFeatureName The action's feature name. If left none, will use the first action.
	 * @param bIsLocalBreak Whether to break the interaction on local machine. If false, use the action's net execution policy instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void StopInteractionWithTarget(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName = NAME_None, bool bIsLocalBreak = false);

	
private:
	void UpdateTagQueryStates(const FGameplayTag& TagEvent = FGameplayTag::EmptyTag);
	bool TryInteractWithTargetInternal(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName);
	void InteractWithTargetInternal(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName);
	void StopInteractWithTargetInternal(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName);
	
	UFUNCTION(Server, Reliable)
	void TryInteractWithTarget_Server(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Server, Reliable)
	void InteractWithTarget_Server(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);
	
	UFUNCTION(NetMulticast, Reliable)
	void InteractWithTarget_Multicast(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);


	UFUNCTION(Server, Reliable)
	void StopInteractionWithTarget_Server(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);
	
	UFUNCTION(NetMulticast, Reliable)
	void StopInteractionWithTarget_Multicast(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInspectSignature OnInspectionStart;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInspectSignature OnInspectionEnd;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInteractSignature OnInteractionStart;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInteractSignature OnInteractionEnd;

protected:
	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	FGameplayTagQuery InspectionEntryTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	FVSGameplayTagEventQueryContainer BreakInspectionTagQueries;

	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	FGameplayTagQuery InteractionEntryTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	FVSGameplayTagEventQueryContainer BreakInteractionTagQueries;
	
private:
	TWeakObjectPtr<UVSGameplayTagController> GameplayTagControllerPrivate;

	/** The target witch the agent is current inspecting. */
	TArray<TWeakObjectPtr<UVSInteractiveFeatureAgent>> CurrentInspectiveAgentsPrivate;

	/** The target witch the agent is current interacting. */
	TWeakObjectPtr<UVSInteractiveFeatureAgent> CurrentInteractiveAgentPrivate;
	FName CurrentInteractiveActionFeatureName = NAME_None;

	uint8 bMatchesInspectionEntryTagQuery : 1;
	uint8 bMatchesInteractionEntryTagQuery : 1;
};
