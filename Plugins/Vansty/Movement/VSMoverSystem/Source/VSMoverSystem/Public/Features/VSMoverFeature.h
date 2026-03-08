// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverSimulationTypes.h"
#include "Features/VSMovementFeatureBase.h"
#include "VSMoverFeature.generated.h"

struct FMover_ProcessGeneratedMovement;
class UVSMoverFeatureAgent;

/**
 * Base movement feature that exposes UMoverComponent data and post-generation hooks.
 */
UCLASS(Abstract, DisplayName = "VS.Feature.Movement.Mover.Base")
class VSMOVERSYSTEM_API UVSMoverFeature : public UVSMovementFeatureBase, public IMoverInputProducerInterface
{
	GENERATED_UCLASS_BODY()
	
	friend class UVSMoverFeatureAgent;

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.

	//~ Begin UVSMovementFeatureBase Interface
	virtual FVector GetAngularVelocity_Implementation() const override;
	virtual FVector GetAcceleration_Implementation() const override;
	virtual FVector GetMovementInput_Implementation() const override;
	virtual USceneComponent* GetUpdatedComponent_Implementation() const override;
	virtual UPrimitiveComponent* GetMovementBase_Implementation() const override;
	virtual FName GetMovementBaseBoneName_Implementation() const override;
	virtual FGameplayTag GetMovementMode_Implementation() const override;
	//~ End UVSMovementFeatureBase Interface

	/** 
	 * Updates proposed movement in the ProcessGeneratedMovement chain before simulation applies it.
	 * Implementations may modify OutProposedMove in-place.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Mover")
	void UpdateGeneratedMovement(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Mover")
	UVSMoverFeatureAgent* GetMoverFeatureAgent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Mover")
	UMoverComponent* GetMoverComponent() const;

private:
	TWeakObjectPtr<UVSMoverFeatureAgent> MoverFeatureAgentPrivate;
};


/** ------------------------------------------------------------------------- **/


/**
 * Agent feature that intercepts mover delegates and forwards them to sub-features.
 */
UCLASS(Abstract, DisplayName = "VS.Feature.Movement.Mover.Agent")
class VSMOVERSYSTEM_API UVSMoverFeatureAgent : public UVSMoverFeature
{
	GENERATED_UCLASS_BODY()

	friend class UVSMoverFeature;

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	//~ End UVSObjectFeature Interface.

	//~ Begin IVSGameplayTagFeatureInterface
	virtual void OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature) override;
	virtual void OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents) override;
	//~ End IVSGameplayTagFeatureInterface
	
	//~ Begin IMoverInputProducerInterface
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;
	//~ End IMoverInputProducerInterface
	
	//~ Begin UVSMovementFeatureBase Interface
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	//~ End UVSMovementFeatureBase Interface

private:
	/** Calls ProduceInput on an object via reflection without static interface dispatch. */
	void CallProduceInput_NoExecute(UObject* Obj, int32 SimTimeMs, FMoverInputCmdContext& InOutCmd);
	
	/** Bound to UMoverComponent::ProcessGeneratedMovement to inject feature-level post processing. */
	UFUNCTION()
	void OnMoverGenerateMovement(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove);
	
	/** Handles movement-mode change notifications from UMoverComponent. */
	UFUNCTION()
	void OnMoverMovementModeChanged(const FName& PreviousMovementModeName, const FName& NewMovementModeName);
	
private:
	TWeakObjectPtr<UMoverComponent> MoverComponentPrivate;
	/** Input producer that was bound before this agent took ownership. */
	TWeakObjectPtr<UObject> OriginalInputProducerPrivate;

	FVector RealAcceleration = FVector::ZeroVector;
	FVector RealAngularVelocity = FVector::ZeroVector;

	FVector LastUpdateVelocity = FVector::ZeroVector;
	FRotator LastUpdateRotation = FRotator::ZeroRotator;

	/** Delegate bound on mover before the agent replaced ProcessGeneratedMovement. */
	TSharedPtr<FMover_ProcessGeneratedMovement> OriginalProcessGeneratedMovement;
	/** Snapshot of ProcessGeneratedMovement right after this agent binds OnMoverGenerateMovement. */
	TSharedPtr<FMover_ProcessGeneratedMovement> ThisProcessGeneratedMovement;
};
