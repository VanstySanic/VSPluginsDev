// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverSimulationTypes.h"
#include "Classes/VSObjectFeature.h"
#include "VSMovementFeature.generated.h"

class UVSMovementFeatureAgent;

/**
 * 
 */
UCLASS()
class VSMOVEMENTSYSTEM_API UVSMovementFeature : public UVSObjectFeature, public IMoverInputProducerInterface
{
	GENERATED_UCLASS_BODY()
	
	friend class UVSMovementFeatureAgent;

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.
	
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UVSMovementFeatureAgent* GetMovementFeatureAgent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UMoverComponent* GetMoverComponent() const;

private:
	TWeakObjectPtr<UVSMovementFeatureAgent> MovementFeatureAgentPrivate;
};


/** ------------------------------------------------------------------------- **/


/**
 * 
 */
UCLASS()
class VSMOVEMENTSYSTEM_API UVSMovementFeatureAgent : public UVSMovementFeature
{
	GENERATED_UCLASS_BODY()

	friend class UVSMovementFeature;

protected:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	//~ End UVSObjectFeature Interface.
	
	//~ Begin IMoverInputProducerInterface
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;
	//~ End IMoverInputProducerInterface

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UObject* GetMoverOriginalInputProducer() const;
	
	
private:
	TWeakObjectPtr<UMoverComponent> MoverComponentPrivate;
	TWeakObjectPtr<UObject> OriginalInputProducerPrivate;
};