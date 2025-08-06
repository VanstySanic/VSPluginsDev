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
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSInteractFeatureAgent* GetInteractFeatureAgent() const { return InteractFeatureAgentPrivate.Get(); }
	
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSGameplayTagController* GetGameplayTagController() const;

	/** Get the target witch the agent is currently inspecting. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	UVSInteractiveFeatureAgent* GetCurrentInspectiveFeatureAgent() const;
	
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
	bool IsInspecting() const;
	
	UFUNCTION(BlueprintCallable, Category = "Interact")
	bool IsInteracting() const;

protected:
	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive")
	FGameplayTagQuery AvailableEntrySourceTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive")
	FGameplayTagQuery AvailableBlockSourceTagQuery;
	
private:
	TWeakObjectPtr<UVSInteractFeatureAgent> InteractFeatureAgentPrivate;
};
