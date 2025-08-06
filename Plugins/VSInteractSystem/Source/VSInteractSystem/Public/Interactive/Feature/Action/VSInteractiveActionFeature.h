// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactive/Feature/VSInteractiveFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSInteractiveActionFeature.generated.h"

class UVSInteractFeatureAgent;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Feature.Interactive.Action.Base")
class VSINTERACTSYSTEM_API UVSInteractiveActionFeature : public UVSInteractiveFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSInteractiveFeatureAgent;

protected:
	virtual void Tick_Implementation(float DeltaTime) override;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactive")
	bool IsAvailableForSource(UVSInteractFeatureAgent* SourceAgent) const;

	UFUNCTION(BlueprintCallable, Category = "Interactive")
	void EndInteraction(UVSInteractFeatureAgent* SourceAgent, bool bIsLocalBreak);
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Interactive")
	void OnStartInteraction(UVSInteractFeatureAgent* SourceAgent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interactive")
	void OnProcessInteraction(UVSInteractFeatureAgent* SourceAgent, const float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Interactive")
	void OnStopInteraction(UVSInteractFeatureAgent* SourceAgent);
	
public:
	/** Works as a switch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bAllowInteraction = true;
	
	/**
	 * Whether the action is instant and has no duration.
	 * If so, OnProcessInteraction of zero delta time will be immediately called once after OnStartInteraction,
	 * and then the interaction will end.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bIsInstantAction = false;

	/**
	 * If not an instance action, this will be the interaction's duration.
	 * If <= 0.f, the action will not automatically end, and you need to manage it manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive", meta = (EditCondition = "!bIsInstantAction"))
	float InteractionDuration = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	int32 MaxInteractionCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	int32 MaxInteractionCountForSource = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	int32 MaxSimultaneousInteractionNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	float InteractionCoolDown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	float InteractionSourceCoolDown = 0.f;

	/**
	 * By default, the cool down will start after the interaction is ended.
	 * If true, cool down will start at the start of the interaction.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bCoolDownAtInteractionStart = false;

	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	FGameplayTagQuery AvailableEntrySourceTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	FGameplayTagQuery AvailableBlockSourceTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive")
	FVSGameplayTagEventQueryContainer BreakInteractionSourceTagQueries;
	
	/** Defines how the action is executed with network. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	FVSNetMethodExecutionPolicies NetExecutionPolicies = FVSNetMethodExecutionPolicies::AutonomousPredictedMulticast;

private:
	int32 CurrentInteractionCount = 0;
	float RemainedInteractionCoolDown = 0.f;
	TMap<TWeakObjectPtr<UVSInteractFeatureAgent>, int32> SourceInteractionCountMap;
	TMap<TWeakObjectPtr<UVSInteractFeatureAgent>, float> InteractingSourceTimeMap;
	TMap<TWeakObjectPtr<UVSInteractFeatureAgent>, float> InteractingSourceRemainedCoolDownMap;
};
