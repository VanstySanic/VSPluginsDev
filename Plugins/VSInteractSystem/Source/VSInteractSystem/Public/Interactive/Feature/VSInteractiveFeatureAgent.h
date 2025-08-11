// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSInteractiveFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSInteractiveFeatureAgent.generated.h"

class UVSInteractiveActionFeature;
class UVSInteractFeature;

/**
 * 
 */
UCLASS(DisplayName = "Feature.Interactive.Agent")
class VSINTERACTSYSTEM_API UVSInteractiveFeatureAgent : public UVSInteractiveFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSInteractiveFeature;
	friend class UVSInteractFeatureAgent;
	friend class UVSInteractiveActionFeature;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInspectSignature, UVSInteractFeatureAgent*, SourceAgent);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractSignature, UVSInteractFeatureAgent*, SourceAgent, const FName, ActionFeatureName);

protected:
	virtual void EndPlay_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	UVSInteractiveActionFeature* GetActionFeatureByName(const FName ActionFeatureName) const;

	UFUNCTION(BlueprintCallable, Category = "Interactive")
	TArray<UVSInteractiveActionFeature*> GetAvailableActionFeatures(UVSInteractFeatureAgent* SourceAgent) const;
	
	/** Whether the agent is inspectable. */
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	bool IsInspectable(UVSInteractFeatureAgent* SourceAgent) const;
	
	/** Whether the agent is interactable. This means the interaction is allowed and there are available actions. */
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	bool IsInteractable(UVSInteractFeatureAgent* SourceAgent) const;
	
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	bool IsActionAvailable(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName) const;

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInspectSignature OnInspectionStart;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInspectSignature OnInspectionEnd;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInteractSignature OnInteractionStart;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnInteractSignature OnInteractionEnd;
	
public:
	/** Works as an internal switch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bAllowInspection = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool InspectRequireInteractivity = true;

	/** Works as an internal switch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bAllowInteraction = true;

	
protected:
	virtual void Initialize_Implementation() override;

private:
	void OnInspectBySource(UVSInteractFeatureAgent* SourceAgent);
	void OnStopInspectBySource(UVSInteractFeatureAgent* SourceAgent);
	void OnInteractBySource(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName);
	void OnStopInteractionBySource(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName);

private:
	TArray<TWeakObjectPtr<UVSInteractFeatureAgent>> InspectingSources;
	TMap<TWeakObjectPtr<UVSInteractFeatureAgent>, FName> InteractingSourceActionMap;
};
